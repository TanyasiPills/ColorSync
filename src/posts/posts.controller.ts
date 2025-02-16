import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Req, ParseIntPipe, HttpCode, NotFoundException, Query } from '@nestjs/common';
import { PostsService } from './posts.service';
import { CreatePostDto } from './dto/create-post.dto';
import { UpdatePostDto } from './dto/update-post.dto';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { ApiBearerAuth, ApiParam, ApiQuery, ApiResponse, getSchemaPath } from '@nestjs/swagger';
import { PostBaseType, PostIncludesType } from 'src/api.dto';

@Controller('posts')
export class PostsController {
  constructor(private readonly postService: PostsService) { }

  /**
   * Create a post from the logged in user
   * @param createPostDto Data about the post
   * @returns The created post's data
   */
  @ApiResponse({ status: 201, description: 'Post sucessfully created' })
  @ApiResponse({ status: 401, description: 'Invalid token' })
  @ApiBearerAuth()

  @UseGuards(JwtAuthGuard)
  @HttpCode(201)
  @Post()
  create(@Body() createPostDto: CreatePostDto, @Req() req: any) {
    return this.postService.create(createPostDto, req.user.id);
  }

  /**
   * Returns all of the posts
   * @param lastId the id of the last post you got
   * @returns {data: PostResponse[], lastId: number} The id of the last post and the data of the posts
   */
  @ApiQuery({ name: 'lastId', description: 'The id of the last post you got', required: false })
  @ApiQuery({ name: 'take', description: 'The amount of posts to take', required: false })
  @ApiResponse({ status: 200, description: 'Returns the posts and the last id', schema: { type: 'object', properties: { data: { type: 'array', items: { $ref: getSchemaPath(PostIncludesType) } }, lastId: { type: 'integer' } } } })
  @Get()
  findAll(@Query('take') take: string, @Query('lastId') lastId: string) {
    return this.postService.findAll(lastId, take);
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
  async findOne(@Param('id', ParseIntPipe) id: number) {
    const result = await this.postService.findOne(id);
    if (result) return result;
    else throw new NotFoundException(`Post with id: ${id} not found`)
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
}
