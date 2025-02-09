import { Injectable, NotFoundException } from '@nestjs/common';
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

  findAll() {
    return this.db.post.findMany({
      orderBy: { date: 'desc' },
      include: { 
        user: { select: { username: true } }, 
        comments: {
          select: {id: true, text: true, date: true, userId: true, user: { select: { username: true } } },
        } 
      } 
      });
  }

  findOne(id: number) {
    return this.db.post.findUnique({ where: { id } });
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
