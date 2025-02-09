import { Injectable, Req, UseGuards } from '@nestjs/common';
import { CreateCommentDto } from './dto/create-comment.dto';
import { UpdateCommentDto } from './dto/update-comment.dto';
import { PrismaService } from 'src/prisma.service';

@Injectable()
export class CommentsService {
  constructor(private readonly db: PrismaService) {}

  create(createCommentDto: CreateCommentDto, userId: number) {
    return "";
  }

  findAllOnPost(postId: number) {
    return `This action returns all comments`;
  }

  findOne(id: number) {
    return `This action returns a #${id} comment`;
  }

  update(id: number, updateCommentDto: UpdateCommentDto, userId: number) {
    return `This action updates a #${id} comment`;
  }

  remove(id: number, userId: number) {
    return `This action removes a #${id} comment`;
  }
}
