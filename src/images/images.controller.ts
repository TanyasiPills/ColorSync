import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Req, HttpCode, HttpException, HttpStatus, UploadedFile, UseInterceptors, ParseIntPipe, Res, NotFoundException } from '@nestjs/common';
import { ImagesService } from './images.service';
import { CreateImageDto } from './dto/create-image.dto';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { FileInterceptor } from '@nestjs/platform-express';
import { diskStorage } from 'multer';
import { extname } from 'path';
import { ApiBearerAuth, ApiConsumes, ApiBody, ApiResponse, ApiParam } from '@nestjs/swagger';
import { ImageAPIType } from 'src/users/dto/api.dto';

@Controller('images')
export class ImagesController {
  constructor(private readonly imageService: ImagesService) { }

  /**
   * Uploades a image
   * @param createImageDto The data to upload the image
   * @param file The file to upload
   */
  @ApiBearerAuth()
  @ApiConsumes('multipart/form-data')
  @ApiBody({ description: 'The ', type: ImageAPIType })
  @ApiResponse({ status: 204, description: 'Image uploaded' })
  @ApiResponse({ status: 401, description: 'Invalid token' })

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

    if (!this.imageService.create(createImageDto, file, req.user.id)) {
      throw new HttpException('Image upload failed!.', HttpStatus.INTERNAL_SERVER_ERROR);
    }
  }
  
  /**
   * Returns the images uploaded by a specific user that the requresting user has access to
   * @param id The id of the user
   * @returns Array of images
   */
  @ApiResponse({status: 200, description: "Returns all the images"})
  @ApiResponse({status: 401, description: "Invalid token"})
  @ApiResponse({status: 404, description: "User not foud"})
  @ApiBearerAuth()
  @ApiParam({name: "id", description: "The id of the user"})

  @UseGuards(JwtAuthGuard)
  @Get('user/:id')
  findAll(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    return this.imageService.findAll(id, req.user.id);
  }

  /**
   * Returns a image file
   * @param id The id of the image
   */
  @ApiResponse({status: 200, description: "Returns the image file"})
  @ApiResponse({status: 401, description: "Unauthorized"})
  @ApiResponse({status: 404, description: "Image not found"})
  @ApiBearerAuth()
  @ApiParam({name: "id", description: "The id of the image"})

  @UseGuards(JwtAuthGuard)
  @Get(':id')
  async findOne(@Param('id', ParseIntPipe) id: number, @Req() req: any, @Res() res: any) {
    const path = await this.imageService.findOne(id, req.user.id);
    if (!path) {
      throw new NotFoundException('Image not found');
    }
    res.sendFile(path);
  }

  /**
   * Returns a public image file
   * @param id The id of the image
   */
  @ApiResponse({status: 200, description: "Returns the image file"})
  @ApiResponse({status: 401, description: "The image is private"})
  @ApiResponse({status: 404, description: "Image not found"})
  @ApiParam({name: "id", description: "The id of the image"})

  @Get('public/:id')
  async findPublic(@Param('id', ParseIntPipe) id: number, @Res() res: any) {
    const path = await this.imageService.findOne(id, -1);
    if (!path) {
      throw new NotFoundException("Image not found");
    }
    res.sendFile(path);
  }

  /**
   * Deletes a image
   * @param id The id of the image
   */
  @ApiResponse({status: 204, description: "Image successfully deleted"})
  @ApiResponse({status: 401, description: "Invalid token"})
  @ApiResponse({status: 404, description: "Image not found, or the user can't delete that image"})
  @ApiBearerAuth()
  @ApiParam({name: "id", description: "The id of the image"})

  @UseGuards(JwtAuthGuard)
  @Delete(':id')
  @HttpCode(204)
  async remove(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    const response = await this.imageService.remove(id, req.user.id);
  }

  /**
   * Updates data about a image
   * @param id The id of the image
   * @param dto The data to update
   * @returns The updated image data
   */
  @ApiResponse({status: 200, description: "Image data succesfully updated"})
  @ApiResponse({status: 401, description: "Invalid token"})
  @ApiResponse({status: 404, description: "Image not found that the user can delete"})
  @ApiBearerAuth()
  @ApiParam({name: "id", description: "The id of the image"})

  @UseGuards(JwtAuthGuard)
  @Patch(':id')
  update(@Param('id', ParseIntPipe) id: number, @Body() dto: CreateImageDto, @Req() req: any) {
    if (dto.visibility == null) throw new HttpException('Visibility is required', HttpStatus.BAD_REQUEST);
    return this.imageService.update(id, req.user.id, dto.visibility);
  }
}
