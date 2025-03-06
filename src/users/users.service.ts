import { Injectable, InternalServerErrorException } from '@nestjs/common';
import { UpdateUserDto } from './dto/update-user.dto';
import { PrismaService } from 'src/prisma.service';
import { existsSync, unlinkSync } from 'fs';
import { resolve } from 'path';

@Injectable()
export class UsersService {
  constructor (private readonly db: PrismaService) {}
  
  findOne(id: number) {
    return this.db.user.findUnique({where: {id}, select: {id: true, username: true}});
  }

  update(id: number, updateUserDto: UpdateUserDto) {
    return this.db.user.update({where: {id}, data: updateUserDto, select: {username: true, email: true}});
  }

  search(name: string) {
    return this.db.user.findMany({where: {username: {contains: name}}, select: {id: true, username: true}, take: 20});
  }

  async remove(id: number) {
    try {
      await this.db.user.delete({where: {id}});
      return true;
    } catch {
      return false;
    }
  }

  async getLikes(id: number) {
    const res = await this.db.user.findUnique({where: {id}, select: {likes: true}});
    if (!res) return [];
    const likes = res.likes.map(e => e.id);
    return likes;
  }

  async upload(file: Express.Multer.File, id: number) {
    const user = await this.db.user.findUnique({where: {id}});
    if (!user) {
      return false;
    }
    if (user.profile_picture) {
      const path = resolve(`uploads/${user.profile_picture}`);
      try {
        if (existsSync(path)) {
          unlinkSync(path);
        }
      } catch (error) {
        console.log(error);
        throw new InternalServerErrorException(error);
      }
    }
    await this.db.user.update({where: {id}, data: {profile_picture: file.filename}});
  }

  async getPfp(id: number) {
    const picture = await this.db.user.findUnique({where: {id}, select: {profile_picture: true}});
    if (!picture) return null;

    let path;
    if (picture.profile_picture == null) path = resolve('files/default_pfp.png');
    else path = resolve(`uploads/${picture.profile_picture}`);
    if (!existsSync(path)) return null;
    return path;
  }

  getLoggedIn(id: number) {
    return this.db.user.findUnique({where: {id}, select: {id: true, username: true, email: true}});
  }
}
