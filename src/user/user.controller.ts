import { LoginBody } from './api.types';
import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Request, HttpCode, HttpException, HttpStatus, ParseIntPipe, UploadedFile, UseInterceptors, Res } from '@nestjs/common';
import { UserService } from './user.service';
import { CreateUserDto } from './dto/create-user.dto';
import { UpdateUserDto } from './dto/update-user.dto';
import { LocalAuthGuard } from 'src/auth/local-auth.guard';
import { AuthService } from 'src/auth/auth.service';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { FileInterceptor } from '@nestjs/platform-express';
import { diskStorage } from 'multer';
import { extname } from 'path';
import { Response } from 'express';
import { ApiBody } from '@nestjs/swagger';

@Controller('user')
export class UserController {
  constructor(
    private readonly userService: UserService,
    private readonly authService: AuthService
  ) {}

  @ApiBody({type: LoginBody})
  @UseGuards(LocalAuthGuard)
  @HttpCode(200)
  @Post('login')
  async login(@Request() req) {
    return this.authService.login(req.user);
  }

  @Post()
  async create(@Body() createUserDto: CreateUserDto) {
    const status = await this.authService.register(createUserDto);
    if (status == true) return this.authService.login(createUserDto);
    else if (status == false) throw new HttpException('Email already in use.', HttpStatus.BAD_REQUEST); 
    else throw new HttpException('Error creating user.', HttpStatus.INTERNAL_SERVER_ERROR);
  }

  @Post('pfp')
  @UseGuards(JwtAuthGuard)
  @UseInterceptors(
    FileInterceptor('file', {
      storage: diskStorage({
        destination: './uploads',
        filename: (req, file, callback) => {
          const uniqueSuffix = Date.now() + '-' + Math.round(Math.random() * 1e9);
          callback(null, `${uniqueSuffix}${extname(file.originalname)}`);
        }
      }),
      fileFilter: (req, file, callback) => {
        if (!file.mimetype.startsWith('image/')) {
          return callback(new Error('Only image files are allowed!'), false);
        }
        callback(null, true);
      }
    })
  )
  uploadPfp(@UploadedFile() file: Express.Multer.File, @Request() req) {
    if (!file) {
      throw new HttpException('File upload failed!.', HttpStatus.BAD_REQUEST);
    }

    this.userService.upload(file, req.user.userId);

    return {filename: file.filename};
  }

  @Get(':id/pfp')
  async getPfp(@Param('id', ParseIntPipe) id: number, @Res() res: Response) {
    const path = await this.userService.getPfp(id);
    if (path) {
      res.sendFile(path);
      return;
    } else {
      return;
    }
  }
  
  @UseGuards(JwtAuthGuard)
  @Get()
  test(@Request() req: any) {
    return req.user;
  }
  
  @Get(':id')
  findOne(@Param('id', ParseIntPipe) id: number) {
    return this.userService.findOne(+id);
  }

  @Patch(':id')
  update(@Param('id', ParseIntPipe) id: number, @Body() updateUserDto: UpdateUserDto) {
    return this.userService.update(+id, updateUserDto);
  }

  @Delete(':id')
  remove(@Param('id', ParseIntPipe) id: number) {
    return this.userService.remove(+id);
  }
}
