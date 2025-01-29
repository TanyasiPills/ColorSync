import { Injectable } from '@nestjs/common';
import { UpdateUserDto } from './dto/update-user.dto';
import { PrismaService } from 'src/prisma.service';
import { unlink } from 'fs';

@Injectable()
export class UserService {
  constructor (private readonly db: PrismaService) {}
  
  findOne(id: number) {
    return this.db.user.findUnique({where: {id}, select: {id: true, username: true, profile_picture: true}});
  }

  update(id: number, updateUserDto: UpdateUserDto) {
    return this.db.user.update({where: {id}, data: updateUserDto});
  }

  remove(id: number) {
    return this.db.user.delete({where: {id}});
  }

  async upload(file: Express.Multer.File, id: number) {
    const user = await this.db.user.findUnique({where: {id}});
    //unlink(); wip
  }
}
