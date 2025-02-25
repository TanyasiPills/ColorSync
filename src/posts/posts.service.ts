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
  }

  async findAll(lastId: string, take: string) {
    let parsedLastid;
    if (lastId) {
      parsedLastid = parseInt(lastId);
      if (isNaN(parsedLastid)) throw new BadRequestException('lastId must be a number');
    }
    let parsedTake;
    if (take) {
      parsedTake = Math.max(1, Math.min(10, parseInt(take)));
      if (isNaN(parsedTake)) throw new BadRequestException('take must be a number');
    } else parsedTake = 10;
    const data: any = await this.db.post.findMany({
      take: parsedTake,
      skip: parsedLastid ? 1 : undefined,
      cursor: parsedLastid ? { id: parsedLastid } : undefined,
      orderBy: { date: 'desc' },
      select: {
        id: true, text: true, date: true, imageId: true,
        user: { select: { username: true, id: true } },
        comments: {
          select: { id: true, text: true, date: true, user: { select: { username: true, id: true } } },
        },
        tags: true
      }
    }
    );

    data.map(e => e.tags = e.tags.map(e => e.name));

    if (data.length === 0) return { data, newLastId: null };
    const newLastId = data[data.length - 1].id;
    return { data, newLastId };
  }

  testSearch() {
    this.elastic.searchPosts("eat", ["cat", "funny"], {queries: [{text: "yo", weight: 1}], tags: [{name: "funny", weight: 1}]});
  }

  async search(tags: string[], take: string, lastId: string) {
    let parsedLastId;
    if (lastId) {
      parsedLastId = parseInt(lastId);
      if (isNaN(parsedLastId)) throw new BadRequestException('lastId must be a number');
    }
    let parsedTake;
    if (take) {
      parsedTake = Math.max(1, Math.min(10, parseInt(take)));
      if (isNaN(parsedTake)) throw new BadRequestException('take must be a number');
    } else parsedTake = 10;

    const tagsFilter = tags ? tags.map(e => ({ tags: { some: { name: { contains: e } } } })) : [];

    const data: any = await this.db.post.findMany({
      where: { AND: tagsFilter },
      take: parsedTake,
      skip: parsedLastId ? 1 : undefined,
      cursor: parsedLastId ? { id: parsedLastId } : undefined,
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

    if (data.length === 0) return { data, newLastId: null };
    const newLastId = data[data.length - 1].id;
    return { data, newLastId };
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

      return result;
    } catch {
      return null;
    }
  }

  async remove(id: number, userId: number): Promise<boolean> {
    try {
      const result = await this.db.post.delete({ where: { id, userId } });
      if (result) return true;
      else return false;
    } catch {
      return false;
    }
  }
}
