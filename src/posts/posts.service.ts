import { BadRequestException, Injectable, NotFoundException } from '@nestjs/common';
import { CreatePostDto } from './dto/create-post.dto';
import { UpdatePostDto } from './dto/update-post.dto';
import { PrismaService } from 'src/prisma.service';
import { SearchService } from 'src/search/search.service';

@Injectable()
export class PostsService {
  constructor(
    private readonly db: PrismaService,
    private readonly elastic: SearchService
  ) { }

  async create(createPostDto: CreatePostDto, userId: number) {
    if (createPostDto.imageId) {
      const image = await this.db.image.findUnique({ where: { id: createPostDto.imageId, userId } });
      if (!image) throw new NotFoundException(`Image with id: ${createPostDto.imageId} not found`);
      if (image.visibility == 'private') throw new NotFoundException(`Image with id: ${createPostDto.imageId} is private`);
    }
    const post = await this.db.post.create({
      data:
      {
        userId,
        text: createPostDto.text,
        imageId: createPostDto.imageId,
        tags: {
          connectOrCreate: createPostDto.tags.map(e => ({ where: { name: e }, create: { name: e } }))
        }
      }
    });
    this.elastic.indexPost(post.id, post.text, [], post.date);
  }

  async search(tags: string[], q: string, take: string, offset: string, imageOnly: boolean = false) {
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

    const ids = await this.elastic.searchPosts(q, tags, null, parsedOffset, parsedTake);
    let selectBody;
    if (imageOnly) selectBody = {
      id: true, imageId: true
    };
    else selectBody = {
      id: true, text: true, date: true, imageId: true,
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
    if (!imageOnly) result.map(e => e.tags = e.tags.map(e => e.name));
    result = result.filter(e => e.imageId);
    return { data: result, offset: result.length == 0 ? null : parsedOffset + result.length };
  }

  async findByUser(id: number) {
    const data: any = this.db.post.findMany({
      where: { userId: id },
      orderBy: { date: 'desc' },
      select: {
        id: true, text: true, date: true, imageId: true,
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
    const data: any = this.db.post.findUnique({
      where: { id },
      select: {
        id: true, text: true, date: true, imageId: true,
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
        return true;
      }
      else return false;
    } catch {
      return false;
    }
  }
}
