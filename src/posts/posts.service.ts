import { ConflictException, Injectable, NotFoundException } from '@nestjs/common';
import { CreatePostDto } from './dto/create-post.dto';
import { UpdatePostDto } from './dto/update-post.dto';
import { PrismaService } from 'src/prisma.service';
import { SearchService } from 'src/search/search.service';
import { existsSync, unlinkSync } from 'fs';
import { resolve } from 'path';

@Injectable()
export class PostsService {
  constructor(
    private readonly db: PrismaService,
    private readonly elastic: SearchService
  ) { }

  async create(createPostDto: CreatePostDto, userId: number, uploaded: boolean = false) {
    if (createPostDto.imageId) {
      const image = await this.db.image.findUnique({ where: { id: createPostDto.imageId, userId } });
      if (!image) throw new NotFoundException(`Image with id: ${createPostDto.imageId} not found`);
      if (image.visibility == 'private') {
        if (createPostDto.forcePost) this.db.image.update({ where: { id: createPostDto.imageId }, data: { visibility: 'public' }});
        else throw new ConflictException(`Image with id: ${createPostDto.imageId} is private`); 
      }
    }
    const post: any = await this.db.post.create({
      data:
      {
        userId,
        text: createPostDto.text,
        imageId: createPostDto.imageId,
        tags: createPostDto.tags ? {
          connectOrCreate: createPostDto.tags.map(e => ({ where: { name: e }, create: { name: e } }))
        } : undefined,
        imageForPost: uploaded
      },
      select: {
        id: true, text: true, date: true, imageId: true, likes: true,
        user: { select: { username: true, id: true } },
        comments: {
          select: { id: true, text: true, date: true, user: { select: { username: true, id: true } } }
        },
        tags: true
      }
    });
    if (post.tags) post.tags = post.tags.map(e => e.name);
    this.elastic.indexPost(post.id, post.text, post.tags, post.date);
    return post;
  }

  async search(tags: string[], q: string, take: string, offset: string, imageOnly: boolean = false, userId = undefined) {
    let parsedTake = 10;
    if (take) {
      parsedTake = parseInt(take);
      if (!Number.isFinite(parsedTake)) parsedTake = 10;
    }
    let parsedOffset = 0;
    if (offset) {
      parsedOffset = parseInt(offset);
      if (!Number.isFinite(parsedOffset)) parsedOffset = 0;
    }

    const ids = await this.elastic.searchPosts(q, tags, parsedOffset, parsedTake, userId);
    let selectBody;
    if (imageOnly) selectBody = {
      id: true, imageId: true
    };
    else selectBody = {
      id: true, text: true, date: true, imageId: true, likes: true,
      user: { select: { username: true, id: true } },
      comments: {
        select: { id: true, text: true, date: true, user: { select: { username: true, id: true } } }
      },
      tags: true
    };
    let result = await Promise.all(ids.map(async (e) => this.db.post.findUnique({
      where: { id: e },
      select: selectBody
    })));
    if (!result || result.length == 0) return { data: [], offset: null };
    if (!imageOnly) {
      result.map(e => e.tags = e.tags.map(e => e.name));
    }
    else result = result.filter(e => e.imageId);
    return { data: result, offset: parsedOffset + result.length };
  }

  async findByUser(id: number) {
    const data: any = this.db.post.findMany({
      where: { userId: id },
      orderBy: { date: 'desc' },
      select: {
        id: true, text: true, date: true, imageId: true, likes: true,
        user: { select: { username: true, id: true } },
        comments: {
          select: { id: true, text: true, date: true, user: { select: { username: true, id: true } } },
        },
        tags: true
      }
    });
    data.map(e => e.tags = e.tags.map(e => e.name));

    return data;
  }

  async findOne(id: number) {
    const data: any = await this.db.post.findUnique({
      where: { id },
      select: {
        id: true, text: true, date: true, imageId: true, likes: true,
        user: { select: { username: true, id: true } },
        comments: {
          select: { id: true, text: true, date: true, user: { select: { username: true, id: true } } },
        },
        tags: true
      }
    });
    if (!data) throw new NotFoundException(`Post with id: ${id} not found`);
    data.tags = data.tags.map(e => e.name);

    return data;
  }

  async update(id: number, updatePostDto: UpdatePostDto, userId: number) {
    try {
      let result;
      if (!updatePostDto.tags) result = await this.db.post.update({
        where: { id, userId },
        data: {
          text: updatePostDto.text,
          imageId: updatePostDto.imageId
        }
      });
      else result = await this.db.post.update({
        where: { id, userId },
        data: {
          text: updatePostDto.text,
          imageId: updatePostDto.imageId,
          tags: {
            set: [],
            connectOrCreate: updatePostDto.tags.map(e => ({ where: { name: e }, create: { name: e } }))
          }
        }, include: { tags: true }
      });
      result.map(e => e.tags = e.tags.map(e => e.name));
      await this.elastic.updatePost(id, updatePostDto.text, updatePostDto.tags, undefined);

      return result;
    } catch {
      return null;
    }
  }

  async remove(id: number, userId: number): Promise<boolean> {
    try {
      const result = await this.db.post.delete({ where: { id, userId } });
      if (result) {
        await this.elastic.deletePost(id);
        if (result.imageForPost) {
          const image = await this.db.image.delete({ where: { id: result.imageId } });
          const path = resolve(image.path);
          if (existsSync(path)) unlinkSync(path);
        }
        return true;
      }
      else return false;
    } catch {
      return false;
    }
  }

  async like(id: number, userId: number) {
    const post = await this.db.post.findUnique({ where: { id }, select: { id: true, likedBy: { select: { id: true } } } });
    if (!post) throw new NotFoundException(`Post with id: ${id} not found`);
    if (post.likedBy.find(e => e.id == userId)) {
      await this.db.post.update({
        where: { id },
        data: { likedBy: { disconnect: { id: userId } }, likes: { decrement: 1 } }
      });
    } else {
      await this.db.post.update({
        where: { id },
        data: { likedBy: { connect: { id: userId } }, likes: { increment: 1 } }
      });
    }
  }

}
