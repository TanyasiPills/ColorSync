import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Req, HttpCode, HttpException, HttpStatus, UploadedFile, UseInterceptors, ParseIntPipe, Res } from '@nestjs/common';
import { ImagesService } from './images.service';
import { CreateImageDto } from './dto/create-image.dto';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { FileInterceptor } from '@nestjs/platform-express';
import { diskStorage } from 'multer';
import { extname, resolve } from 'path';

@Controller('images')
export class ImagesController {
  constructor(private readonly imageService: ImagesService) { }

  /**
   * 
   * @param createImageDto The data to create the image
   * @param file The file to upload
   */
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
  @HttpCode(204)
  @Post()
  create(@Body() createImageDto: CreateImageDto, @UploadedFile() file: Express.Multer.File, @Req() req: any) {
    if (!file) {
      throw new HttpException('File upload failed!.', HttpStatus.BAD_REQUEST);
    }

    if(!this.imageService.create(createImageDto, file, req.user.id)) {
      throw new HttpException('Image upload failed!.', HttpStatus.INTERNAL_SERVER_ERROR);
    }
  }

  @UseGuards(JwtAuthGuard)
  @Get('user/:id')
  findAll(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    return this.imageService.findAll(id, req.user.id);
  }

  @UseGuards(JwtAuthGuard)
  @Get(':id')
  async findOne(@Param('id', ParseIntPipe) id: number, @Req() req: any, @Res() res: any) {
    const path = await this.imageService.findOne(id, req.user.id);
    if (!path) {
      throw new HttpException('Image not found', HttpStatus.NOT_FOUND);
    }
    res.sendFile(path);
  }

  @Get('public/:id')
  async findPublic(@Param('id', ParseIntPipe) id: number, @Res() res: any) {
    const path = await this.imageService.findOne(id, -1);
    if (!path) {
      throw new HttpException('Image not found', HttpStatus.NOT_FOUND);
    }
    res.sendFile(path);
  }

  @UseGuards(JwtAuthGuard)
  @Delete(':id')
  remove(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    return this.imageService.remove(id, req.user.id);
  }

  @UseGuards(JwtAuthGuard)
  @Patch(':id')
  update(@Param('id', ParseIntPipe) id: number, @Body() dto: CreateImageDto, @Req() req: any) {
    if (dto.visibility == null) throw new HttpException('Visibility is required', HttpStatus.BAD_REQUEST);
    return this.imageService.update(id, req.user.id, dto.visibility);
  }
}
