import { CreateUserDto } from './../user/dto/create-user.dto';
import { Injectable } from '@nestjs/common';
import { JwtService } from '@nestjs/jwt';
import { PrismaService } from 'src/prisma.service';
import { verify, hash } from "argon2"

@Injectable()
export class AuthService {
  constructor(
    private readonly db: PrismaService,
    private readonly jwtService: JwtService
  ) {}

  async validateUser(id: number, pass: string): Promise<any> {
    const user = await this.db.user.findUnique({where: {id}});
    if (!user) return null;
    if (!verify(user.password, pass)) return null;
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    const {password, ...result} = user;
    return result;
  }

  async login(user: any) {
    const payload = {username: user.username, sub: user.userId};
    const storedToken = this.db.token.findFirst({where: {userId: user.userId}})
    //WIP storedToken
    return {
      acces_token: this.jwtService.sign(payload)
    }
  }

  async register(dto: CreateUserDto) {
    dto.password = await hash(dto.password);
    return this.db.user.create({data: dto});
  }
}
