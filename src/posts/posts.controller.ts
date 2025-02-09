import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Req, ParseIntPipe, HttpCode, NotFoundException } from '@nestjs/common';
import { PostsService } from './posts.service';
import { CreatePostDto } from './dto/create-post.dto';
import { UpdatePostDto } from './dto/update-post.dto';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { ApiParam, ApiResponse } from '@nestjs/swagger';

@Controller('posts')
export class PostsController {
  constructor(private readonly postService: PostsService) {}

  /**
   * Create a post from the logged in user
   * @param createPostDto Data about the post
   * @returns The created post's data
   */
  @ApiResponse({status: 201, description: 'Post sucessfully created'})
  @ApiResponse({status: 401, description: 'Invalid token'})

  @UseGuards(JwtAuthGuard)
  @HttpCode(201)
  @Post()
  create(@Body() createPostDto: CreatePostDto, @Req() req: any) {
    return this.postService.create(createPostDto, req.user.id);
  }

  /**
   * Returns all of the posts
   * @returns Array of the posts
   */
  @ApiResponse({status: 204, description: 'List of posts returned'})
  @Get()
  findAll() {
    return this.postService.findAll();
  }

  /**
   * Returns one post based on it's id
   * @param id the id of the post
   * @returns 
   */
  @ApiParam({name: 'id', description: 'The id of the post'})

  @ApiResponse({status: 204, description: 'Returns the post'})
  @ApiResponse({status: 404, description: 'Post not found'})
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
  @ApiResponse({status: 204, description: 'The post was successfully updated'})
  @ApiResponse({status: 401, description: 'Invalid token'})
  @ApiResponse({status: 404, description: "There is not post with that id or the user can't update it"})

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
  @ApiResponse({status: 204, description: 'The post was successfully deleted'})
  @ApiResponse({status: 404, description: "There is no post with that id or the user can't delete it"})
  @ApiResponse({status: 401, description: 'Invalid token'})

  @UseGuards(JwtAuthGuard)
  @HttpCode(204)
  @Delete(':id')
  async remove(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    const result = this.postService.remove(id, req.user.id); 
    if (!result) throw new NotFoundException(`A post doesn't exist with id: ${id} that the user can delete`)
  }
}
