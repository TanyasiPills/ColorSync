import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Req, HttpCode, HttpException, HttpStatus, UploadedFile, UseInterceptors, ParseIntPipe, Res, NotFoundException, Query, ParseBoolPipe } from '@nestjs/common';
import { ImagesService } from './images.service';
import { CreateImageDto } from './dto/create-image.dto';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { FileInterceptor } from '@nestjs/platform-express';
import { diskStorage } from 'multer';
import { basename, extname, join, resolve } from 'path';
import { ApiBearerAuth, ApiConsumes, ApiBody, ApiResponse, ApiParam, ApiQuery } from '@nestjs/swagger';
import { ImageCreateType, ImageType } from 'src/api.dto';
import { existsSync, mkdirSync, unlinkSync } from 'fs';
import { OptionalAuthGuard } from 'src/auth/optional.guard';
import * as sharp from 'sharp';
import { UpdateImageDto } from './dto/update-image.dto';

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
  @ApiBody({ description: 'The data of the image', type: ImageCreateType })
  @ApiResponse({ status: 200, description: 'Image uploaded', schema: { type: 'object', properties: { id: { type: 'number' } } } })
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
  @HttpCode(200)
  @Post()
  async create(@Body() createImageDto: CreateImageDto, @UploadedFile() file: Express.Multer.File, @Req() req: any) {
    if (!file) {
      throw new HttpException('File upload failed!.', HttpStatus.BAD_REQUEST);
    }

    const result = await this.imageService.create(createImageDto, file, req.user.id);
    if (!result) {
      const path = resolve(file.path);
      if (existsSync(path)) unlinkSync(path);
      throw new HttpException('Image upload failed!.', HttpStatus.INTERNAL_SERVER_ERROR);
    }

    return { id: result.id };
  }

  
  /**
   * Returns the images uploaded by the logged in user
   * @returns {ImageType[]} Array of images
   */
  @ApiResponse({ status: 200, description: "Returns all the images", type: ImageType, isArray: true })
  @ApiResponse({ status: 401, description: "Invalid token" })
  @ApiResponse({ status: 404, description: "No images found" })
  @ApiBearerAuth()

  @UseGuards(JwtAuthGuard)
  @Get()
  getOwnImages(@Req() req) {
    return this.imageService.findAll(req.user.id, req.user.id);
  }

  /**
   * Returns the images uploaded by a specific user that the requresting user has access to
   * @param id The id of the user
   * @returns Array of images
   */
  @ApiResponse({ status: 200, description: "Returns all the images", type: ImageType, isArray: true })
  @ApiResponse({ status: 401, description: "Invalid token" })
  @ApiResponse({ status: 404, description: "User not foud or no images found" })
  @ApiBearerAuth()
  @ApiParam({ name: "id", description: "The id of the user" })

  @UseGuards(OptionalAuthGuard)
  @Get('user/:id')
  findAllPublic(@Param('id', ParseIntPipe) id: number, @Req() req) {
    let userId = -1;
    if (req.user) userId = req.user.id
    return this.imageService.findAll(id, userId);
  }

  /**
  * Returns the image file
  * @param id The id of the image
  */
  @ApiResponse({ status: 200, description: "Returns the image file" })
  @ApiResponse({ status: 401, description: "Unauthorized" })
  @ApiResponse({ status: 404, description: "Image not found" })
  @ApiBearerAuth()
  @ApiParam({ name: "id", description: "The id of the image" })
  @ApiQuery({ name: "maxSize", description: "The maximum size of the image (default=1920)", required: false, type: Number, example: 1920 })

  @UseGuards(OptionalAuthGuard)
  @Get(':id')
  async findOne(@Param('id', ParseIntPipe) id: number, @Req() req: any, @Res() res: any, @Query('maxSize') maxSize: string) {
    let userId = -1;
    if (req.user) userId = req.user.id
    const path = await this.imageService.findOne(id, userId);
    if (!path) {
      throw new NotFoundException('Image not found');
    }
    if (!existsSync(path)){
      console.error("Image file with id: " + id + " doesn't exist");
      throw new NotFoundException("This image file seems to be missing, put up some flyers");
    }

    const image = sharp(path);
    const { width, height, format } = await image.metadata();

    const filename = basename(path);
    const defaultMaxSize = 1920;
    let maxSizeInt = parseInt(maxSize);
    if (isNaN(maxSizeInt) || maxSizeInt <= 0) {
      maxSizeInt = defaultMaxSize;
    }
    
    let cachedPath;
    if (maxSizeInt == defaultMaxSize) {
      const cacheFolder = join(__dirname, '..', '..', 'uploads', 'cache');
      if (!existsSync(cacheFolder)) {
        mkdirSync(cacheFolder, { recursive: true });
      }
      
      cachedPath = join(cacheFolder, filename);

      if (existsSync(cachedPath)) {
        return res.sendFile(cachedPath);
      }
    }

    if (width > maxSizeInt || height > maxSizeInt) {
      const aspectRatio = width / height;
      const newWidth = width > height ? maxSizeInt : Math.floor(maxSizeInt * aspectRatio);
      const newHeight = height > width ? maxSizeInt : Math.floor(maxSizeInt / aspectRatio);
      image.resize(newWidth, newHeight);
    }
    if (maxSizeInt == defaultMaxSize) {
      await image.toFile(cachedPath);
      return res.sendFile(cachedPath);
    } else {
      res.type("image/" + format);
      image.pipe(res);
    }
  }

  /**
   * Deletes a image
   * @param id The id of the image
   */
  @ApiResponse({ status: 204, description: "Image successfully deleted" })
  @ApiResponse({ status: 401, description: "Invalid token" })
  @ApiResponse({ status: 404, description: "Image not found, or the user can't delete that image" })
  @ApiBearerAuth()
  @ApiParam({ name: "id", description: "The id of the image" })

  @UseGuards(JwtAuthGuard)
  @Delete(':id')
  @HttpCode(204)
  async remove(@Param('id', ParseIntPipe) id: number, @Req() req: any, @Query('forceDelete') forceDelete: string) {
    await this.imageService.remove(id, req.user.id, forceDelete == "true");
  }

  /**
   * Updates data about a image
   * @param id The id of the image
   * @param dto The data to update
   * @returns The updated image data
   */
  @ApiResponse({ status: 200, description: "Image data succesfully updated", type: ImageType })
  @ApiResponse({ status: 401, description: "Invalid token" })
  @ApiResponse({ status: 404, description: "Image not found that the user can udpate" })
  @ApiResponse({ status: 409, description: "The image is part of a post, so it can't be updated" })
  @ApiBearerAuth()
  @ApiParam({ name: "id", description: "The id of the image" })

  @UseGuards(JwtAuthGuard)
  @Patch(':id')
  update(@Param('id', ParseIntPipe) id: number, @Body() dto: UpdateImageDto, @Req() req: any) {
    return this.imageService.update(id, req.user.id, dto);
  }
}
