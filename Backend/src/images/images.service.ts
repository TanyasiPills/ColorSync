import { ConflictException, Injectable, NotFoundException, UnauthorizedException } from '@nestjs/common';
import { CreateImageDto } from './dto/create-image.dto';
import { PrismaService } from 'src/prisma.service';
import { Visibility } from '@prisma/client';
import { existsSync, statSync, unlinkSync } from 'fs';
import { resolve } from 'path';
import { UpdateImageDto } from './dto/update-image.dto';
import { SearchService } from 'src/search/search.service';

@Injectable()
export class ImagesService {
  constructor(
    private readonly db: PrismaService,
    private readonly elastic: SearchService,
  ) { }
  async create(createImageDto: CreateImageDto, file: Express.Multer.File, id: number, forPost: boolean = false) {
    try {
      return await this.db.image.create({ data: { ...createImageDto, path: file.filename, user: { connect: { id } }, forPost } });
    } catch {
      return null;
    }
  }

  async findAll(id: number, userId: number) {
    try {
      await this.db.user.findUniqueOrThrow({ where: { id } });
    } catch {
      throw new NotFoundException(`User with id: ${id} not found`);
    }
    let result;
    if (id == userId) {
      result = await this.db.image.findMany({ where: { userId: id, forPost: false }, orderBy: { date: 'desc' }, select: { id: true, date: true, visibility: true, userId: true } });
    } else {
      result = await this.db.image.findMany({
        where: { visibility: Visibility.public, userId: id, forPost: false },
        orderBy: { date: 'desc' },
        select: { id: true, date: true, userId: true, visibility: true }
      });
    }
    if (!result || result.length == 0) throw new NotFoundException("No images found :c");
    else return result;
  }

  async findOne(id: number, userId: number) {
    const image = await this.db.image.findUnique({ where: { id }, select: { path: true, visibility: true, userId: true } });
    if (!image) throw new NotFoundException(`Image with id: ${id} not found`);
    if (image.visibility == Visibility.private && image.userId != userId) throw new UnauthorizedException("You don't have access to this image");
    return resolve(`uploads/${image.path}`);
  }

  async update(id: number, userId: number, dto: UpdateImageDto) {
    const image = await this.db.image.findUnique({ where: { id, userId }, select: { id: true, date: true, visibility: true, userId: true, posts: true } });
    if (!image) throw new NotFoundException(`Image with id: ${id} not found that user: ${userId} can update`);
    let promises: Promise<any>[];
    if (image.posts && image.posts.length > 0) {
      if (dto.forceUpdate) {
        promises = image.posts.map(async (post) => await this.removePost(post.id, userId));
      } else {
        throw new ConflictException("You can't update a image included in a post")
      }
    }
    if (promises) await Promise.all(promises);
    await this.db.image.update({ where: { id, userId }, data: { visibility: dto.visibility } });
  }

  async remove(id: number, userId: number, forceDelete: boolean) {
    const image = await this.db.image.findUnique({ where: { id, userId }, select: { posts: true, path: true } });
    if (!image) throw new NotFoundException(`Image with id: ${id} not found that user: ${userId} can delete`);
    let promises: Promise<any>[];
    if (image.posts && image.posts.length > 0) {
      if (forceDelete) {
        promises = image.posts.map(async (post) => this.removePost(post.id, userId));
      } else {
        throw new ConflictException("You can't delete a image included in a post")
      }
    }
    if (promises) await Promise.all(promises);
    await this.db.image.delete({ where: { id, userId } });
    if (existsSync(`uploads/${image.path}`)) unlinkSync(`uploads/${image.path}`);
    if (`uploads/cache/${image.path}`) unlinkSync(`uploads/cache/${image.path}`);
  }

  async removePost(id: number, userId: number) {
    const post = await this.db.post.findUnique({ where: { id, userId }, select: { imageForPost: true, imageId: true, comments: true} });
    if (!post) throw new NotFoundException(`Post with id: ${id} not found that user: ${userId} can delete`);
    if (post.comments && post.comments.length > 0) {
      await Promise.all(post.comments.map(async (comment) => await this.db.comment.delete({ where: { id: comment.id } })));
    }
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
  }
}
