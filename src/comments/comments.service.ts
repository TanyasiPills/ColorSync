import { Injectable, NotFoundException, Req, UseGuards } from '@nestjs/common';
import { CreateCommentDto } from './dto/create-comment.dto';
import { UpdateCommentDto } from './dto/update-comment.dto';
import { PrismaService } from 'src/prisma.service';

@Injectable()
export class CommentsService {
  constructor(private readonly db: PrismaService) {}

  create(createCommentDto: CreateCommentDto, userId: number) {
    return this.db.comment.create({data: {...createCommentDto, userId}});
  }

  findAllOnPost(postId: number) {
    try {
      return this.db.post.findUniqueOrThrow({where: {id: postId}, select: {comments: true}});
    } catch {
      throw new NotFoundException(`Post with id: ${postId} not found`);
    }
  }

  findOne(id: number) {
    try {
      return this.db.comment.findUniqueOrThrow({where: {id}});
    } catch {
      throw new NotFoundException(`Comment with id: ${id} not found`);
    }
  }

  async update(id: number, updateCommentDto: UpdateCommentDto, userId: number) {
    try {
      return await this.db.comment.update({where: {id, userId}, data: updateCommentDto});
    } catch {
      throw new NotFoundException(`A post with id: ${id} not found for user: ${userId}`);
    }
  }

  async remove(id: number, userId: number) {
    try {
      return await this.db.comment.delete({where: {id, userId}});
    } catch {
      throw new NotFoundException(`A post with id: ${id} not found for user: ${userId}`);
    }
  }
}
