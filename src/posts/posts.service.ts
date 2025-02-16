import { BadRequestException, Injectable, NotFoundException } from '@nestjs/common';
import { CreatePostDto } from './dto/create-post.dto';
import { UpdatePostDto } from './dto/update-post.dto';
import { PrismaService } from 'src/prisma.service';

@Injectable()
export class PostsService {
  constructor(private readonly db: PrismaService) { }
  async create(createPostDto: CreatePostDto, userId: number) {
    if (createPostDto.imageId) {
      const image = await this.db.image.findUnique({ where: { id: createPostDto.imageId, userId } });
      if (!image) throw new NotFoundException(`Image with id: ${createPostDto.imageId} not found`);
      if (image.visibility == 'private') throw new NotFoundException(`Image with id: ${createPostDto.imageId} is private`);
    }
    return this.db.post.create({ data: { userId, ...createPostDto, } });
  }

  async findAll(lastId: string, take: string) {
    let parsedId;
    if (lastId) {
      parsedId = parseInt(lastId);
      if (isNaN(parsedId)) throw new BadRequestException('lastId must be a number');
    }
    let parsedTake;
    if (take) {
      parsedTake = Math.max(1, Math.min(10, parseInt(take)));
      if (isNaN(parsedTake)) throw new BadRequestException('take must be a number');
    } else parsedTake = 10;
    const data = await this.db.post.findMany({
      take: parsedTake,
      skip: parsedId ? 1 : 0,
      cursor: parsedId ? { id: parsedId } : undefined,
      orderBy: { date: 'desc' },
      select: {
        id: true, text: true, date: true, imageId: true,
        user: { select: { username: true, id: true } },
        comments: {
          select: { id: true, text: true, date: true, user: { select: { username: true, id: true } } },
        }
      }}
    );
    if (data.length === 0) return { data, newLastId: null };
    const newLastId = data[data.length - 1].id;
    return { data, newLastId };
  }

  findByUser(id: number) {
    return this.db.post.findMany({
      where: { userId: id },
      orderBy: { date: 'desc' },
      select: {
        id: true, text: true, date: true, imageId: true,
        user: { select: { username: true, id: true } },
        comments: {
          select: { id: true, text: true, date: true, user: { select: { username: true, id: true } } },
        }
      }
    });
  }

  findOne(id: number) {
    return this.db.post.findUnique({
      where: { id },
      select: {
        id: true, text: true, date: true, imageId: true,
        user: { select: { username: true, id: true } },
        comments: {
          select: { id: true, text: true, date: true, user: { select: { username: true, id: true } } },
        }
      }
    });
  }

  async update(id: number, updatePostDto: UpdatePostDto, userId: number) {
    try {
      const result = await this.db.post.update({ where: { id, userId }, data: updatePostDto });
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
