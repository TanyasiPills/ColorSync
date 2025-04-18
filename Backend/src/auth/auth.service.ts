import { CreateUserDto } from '../users/dto/create-user.dto';
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

  async validateUser(email: string, pass: string): Promise<any> {;
    const user = await this.db.user.findUnique({where: {email}});
    if (!user) return null;
    if (!(await verify(user.password, pass))) return null;
    delete user.password;
    return user;
  }

  async validateToken(token: string) {
    try {
      const decoded = this.jwtService.verify(token);
      const user = await this.db.user.findUnique({where: {id: decoded.sub}});
      if (!user) return null;
      else return user;
    } catch {
      return null;
    }
  }

  async login(user: any) {
    const payload = {username: user.username, sub: user.id};
    const token = await this.jwtService.sign(payload);
    return {access_token: token, username: user.username, id: user.id, email: user.email};
  }

  async register(dto: CreateUserDto) {
    try {
      dto.password = await hash(dto.password);
      const user = await this.db.user.create({data: dto, select: {id: true, username: true, email: true}});
      return user;
    } catch (e) {
      if (e.code == 'P2002') return null;
      else return undefined;
    }
  }
}
