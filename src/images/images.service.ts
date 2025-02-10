import { Injectable, NotFoundException, UnauthorizedException } from '@nestjs/common';
import { CreateImageDto } from './dto/create-image.dto';
import { PrismaService } from 'src/prisma.service';
import { Visibility } from '@prisma/client';
import { unlinkSync } from 'fs';
import { resolve } from 'path';

@Injectable()
export class ImagesService {
  constructor(private readonly db: PrismaService) { }
  async create(createImageDto: CreateImageDto, file: Express.Multer.File, id: number) {
    try {
      await this.db.image.create({data: {...createImageDto, path: file.filename, user: {connect: {id}}}});
      return true;
    } catch {
      return false;
    }
  }

  async findAll(id: number, userId: number) {
    try {
      await this.db.user.findUniqueOrThrow({where: {id}});
    } catch {
      throw new NotFoundException(`User with id: ${id} not found`);
    }
    if (id == userId) {
      return this.db.image.findMany({where: {userId: id}, select: {id: true, date: true, visibility: true}});
    } else {
      return this.db.image.findMany({where: {visibility: Visibility.public, userId: id}, select: {id: true, date: true}});
    }
  }

  async findOne(id: number, userId: number) {
    const image = await this.db.image.findUnique({where: {id}});
    if (!image) throw new NotFoundException(`Image with id: ${id} not found`);
    if (image.visibility == Visibility.private && image.userId != userId) throw new UnauthorizedException("You don't have access to this image");
    return resolve(`uploads/${image.path}`);;
  }

  async update(id: number, userId: number, visibility: Visibility) {
    try {
      const image = this.db.image.findUniqueOrThrow({where: {id, userId}, include: {posts: true}});
      if (image.posts) throw new UnauthorizedException("You can't change the visibility of an image that is part of a post");
      await this.db.image.update({where: {id, userId}, data: {visibility}});
    } catch {
      throw new NotFoundException(`Image with id: ${id} not found that user: ${userId} can update`);
    }
  }

  async remove(id: number, userId: number) {
    try {
      const image = await this.db.image.delete({where: {id, userId}});
      unlinkSync(`uploads/${image.path}`);
      return true;
    } catch {
      throw new NotFoundException(`Image with id: ${id} not found that user: ${userId} can delete`);
    }
  }
}
