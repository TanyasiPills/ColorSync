import { Injectable } from '@nestjs/common';
import { CreatePostDto } from './dto/create-post.dto';
import { UpdatePostDto } from './dto/update-post.dto';
import { PrismaService } from 'src/prisma.service';

@Injectable()
export class PostService {
  constructor(private readonly db: PrismaService) {}
  create(createPostDto: CreatePostDto, userId: number) {
    return this.db.post.create({data: {userId, ...createPostDto, }});
  }

  findAll() {
    return this.db.post.findMany();
  }

  findOne(id: number) {
    return this.db.post.findUnique({where: {id}});
  }

  async update(id: number, updatePostDto: UpdatePostDto, userId: number) {
    try {
      const result = await this.db.post.update({where: {id, userId}, data: updatePostDto});
      return result;
    } catch {
      return null;
    }
  }

  async remove(id: number, userId: number): Promise<boolean> {
    try {
      const result = await this.db.post.delete({where: {id, userId}});
      if (result) return true;
      else return false;
    } catch {
      return false;
    }
  }
}
