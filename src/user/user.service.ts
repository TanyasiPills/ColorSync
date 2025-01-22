import { Injectable } from '@nestjs/common';
import { UpdateUserDto } from './dto/update-user.dto';
import { PrismaService } from 'src/prisma.service';

@Injectable()
export class UserService {
  constructor (private readonly db: PrismaService) {}
  
  findOne(id: number) {
    return this.db.user.findUnique({where: {id}});
  }

  update(id: number, updateUserDto: UpdateUserDto) {
    return this.db.user.update({where: {id}, data: updateUserDto});
  }

  remove(id: number) {
    return this.db.user.delete({where: {id}});
  }
}
