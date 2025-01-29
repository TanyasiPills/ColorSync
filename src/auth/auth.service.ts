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
    const storedToken = await this.db.token.findFirst({where: {userId: user.id}})
    if (storedToken) return {access_token: storedToken.token};
    else {
      const token = await this.jwtService.sign(payload);
      await this.db.token.create({data: {token, userId: user.id}});
      return {access_token: token, username: user.username, id: user.id};
    }
  }

  async register(dto: CreateUserDto) {
    try {
      dto.password = await hash(dto.password);
      await this.db.user.create({data: dto, select: {username: true, email: true}});
      return true;
    } catch (e) {
      if (e.code == 'P2002') return false;
      else return undefined;
    }
  }
}
