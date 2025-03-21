import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Req, ParseIntPipe, HttpCode, NotFoundException, Query, UseInterceptors, UploadedFile, Request } from '@nestjs/common';
import { PostsService } from './posts.service';
import { CreatePostDto } from './dto/create-post.dto';
import { UpdatePostDto } from './dto/update-post.dto';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { ApiBearerAuth, ApiBody, ApiConsumes, ApiParam, ApiQuery, ApiResponse, getSchemaPath } from '@nestjs/swagger';
import { PostBaseType, PostIncludesType, PostInputBodyType } from 'src/api.dto';
import { FileInterceptor } from '@nestjs/platform-express';
import { diskStorage } from 'multer';
import { extname } from 'path';
import { ImagesService } from 'src/images/images.service';
import { OptionalAuthGuard } from 'src/auth/optional.guard';

@Controller('posts')
export class PostsController {
  constructor(
    private readonly postService: PostsService,
    private readonly imageService: ImagesService
  ) { }

  /**
   * Create a post for the logged in user
   * @param createPostDto Data about the post
   * @returns The created post's data
   */
  @ApiConsumes('multipart/form-data')
  @ApiBody({description: "The data of the image, you can send either a image id for the image or a image file to upload", type: PostInputBodyType})
  @ApiResponse({ status: 201, description: 'Post sucessfully created', type: PostIncludesType })
  @ApiResponse({ status: 401, description: 'Invalid token' })
  @ApiResponse({ status: 409, description: "Can't post private image" })
  @ApiBearerAuth()

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
  @HttpCode(201)
  @Post()
  async create(@Body() createPostDto: CreatePostDto, @Req() req: any, @UploadedFile() file: Express.Multer.File) {
    let uploaded = false;
    if (file) {
      const image = await this.imageService.create({visibility: 'public'}, file, req.user.id, true);
      createPostDto.imageId = image.id;
      uploaded = true;
    }
  
    return await this.postService.create(createPostDto, req.user.id, uploaded);
  }


  /**
   * Returns all of the posts
   * @param take The amount of posts to return at one time
   * @param offset the id of the last post you got
   * @returns {data: PostResponse[], offset: number} The id of the last post and the data of the posts
   */
  @ApiQuery({ name: 'offset', description: 'The id of the last post you got', required: false })
  @ApiQuery({ name: 'take', description: 'The amount of posts to take', required: false, minimum: 1, maximum: 10 })
  @ApiResponse({ status: 200, description: 'Returns the posts and the last id', schema: { type: 'object', properties: { data: { type: 'array', items: { $ref: getSchemaPath(PostIncludesType) } }, offset: { type: 'integer' } } } })
  @ApiBearerAuth()

  @UseGuards(OptionalAuthGuard)
  @Get()
  findAll(@Query('take') take: string, @Query('offset') offset: string, @Req() req: any) {
    return this.postService.search(null, null, take, offset, false, req.user ? req.user.id : undefined);
  }

  /**
   * Searchs for posts with specified tags included
   * @param tags The tags to search for
   * @param take The amount of posts to return at one time
   * @param offset The lastId from the previous request
   * @returns {data: PostResponse[], offset: number} The id of the last post and the data of the posts
   */
  @ApiQuery({ name: 'q', description: 'The text to search for', required: false })
  @ApiQuery({ name: 'tags', description: 'The tags to search for', required: false })
  @ApiQuery({ name: 'offset', description: 'The id of the last post you got', required: false })
  @ApiQuery({ name: 'take', description: 'The amount of posts to take', required: false, minimum: 1, maximum: 10 })
  @ApiQuery({ name: 'imageOnly', description: 'Only imageid and id is returned, default is 1', required: false })
  @ApiResponse({ status: 200, description: 'Returns the posts and the last id', schema: { type: 'object', properties: { data: { type: 'array', items: { $ref: getSchemaPath(PostIncludesType) } }, offset: { type: 'integer' } } } })
  @ApiBearerAuth()

  @UseGuards(OptionalAuthGuard)
  @Get('search') 
  search(@Query('tags') tags: string[], @Query('q') q: string, @Query('take') take: string, @Query('offset') offset: string, @Query('imageOnly') imageOnly: string = "1", @Req() req: any) {
    if (!tags) tags = [];
    else if (!Array.isArray(tags)) tags = [tags];
    return this.postService.search(tags, q, take, offset, imageOnly == '1', req.user ? req.user.id : undefined);
  }

  /**
   * Returns all of the posts created by a specific user
   * @param id The id of the user
   * @returns {PostIncludesType[]} Array of posts
   */
  @ApiParam({ name: 'id', description: 'The id of the user' })
  @ApiResponse({ status: 200, description: 'Returns the posts', type: PostIncludesType, isArray: true })

  @Get('user/:id')
  findByUser(@Param('id', ParseIntPipe) id: number) {
    return this.postService.findByUser(id);
  }

  /**
   * Returns one post based on it's id
   * @param id the id of the post
   * @returns 
   */
  @ApiParam({ name: 'id', description: 'The id of the post' })
  @ApiResponse({ status: 404, description: 'Post not found' })
  @ApiResponse({ status: 200, description: 'Returns the post', type: PostIncludesType })

  @Get(':id')
  findOne(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    return this.postService.findOne(id);
  }


  /**
   * Update a specific post create by the user
   * @param id the id of the post
   * @param updatePostDto the data to update in the post
   * @returns the updated post
   */
  @ApiResponse({ status: 200, description: 'The post was successfully updated', type: PostBaseType })
  @ApiResponse({ status: 401, description: 'Invalid token' })
  @ApiResponse({ status: 404, description: "There is not post with that id or the user can't update it"})
  @ApiBearerAuth()
  @ApiParam({ name: "id", description: "The id of the post" })

  @UseGuards(JwtAuthGuard)
  @Patch(':id')
  async update(@Param('id', ParseIntPipe) id: number, @Body() updatePostDto: UpdatePostDto, @Req() req: any) {
    const result = await this.postService.update(id, updatePostDto, req.user.id);
    if (!result) throw new NotFoundException(`A post doesn't exit with id: ${id} that the user can update`);
    return result;
  }

  /**
   * Deletes a specific post
   * @param id the id of the post
   * @returns
   */
  @ApiResponse({ status: 204, description: 'The post was successfully deleted' })
  @ApiResponse({ status: 404, description: "There is no post with that id or the user can't delete it" })
  @ApiResponse({ status: 401, description: 'Invalid token' })
  @ApiBearerAuth()
  @ApiParam({ name: "id", description: "The id of the post" })

  @UseGuards(JwtAuthGuard)
  @HttpCode(204)
  @Delete(':id')
  async remove(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    const result = await this.postService.remove(id, req.user.id);
    if (!result) throw new NotFoundException(`A post doesn't exist with id: ${id} that the user can delete`)
  }


  /**
   * Like a post
   * @param id the id of the post
   * @returns
   */
  @ApiResponse({ status: 204, description: 'The post was successfully liked' })
  @ApiResponse({ status: 401, description: 'Invalid token' })
  @ApiResponse({ status: 404, description: 'There is no post with that id' })
  @ApiParam({ name: 'id', description: 'The id of the post' })
  @ApiBearerAuth()
  
  @UseGuards(JwtAuthGuard)
  @HttpCode(204)
  @Post('like/:id')
  async like(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    await this.postService.like(id, req.user.id);
  }
}
