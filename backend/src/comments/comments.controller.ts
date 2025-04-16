import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Req, ParseIntPipe, HttpCode } from '@nestjs/common';
import { CommentsService } from './comments.service';
import { CreateCommentDto } from './dto/create-comment.dto';
import { UpdateCommentDto } from './dto/update-comment.dto';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { ApiBearerAuth, ApiParam, ApiResponse } from '@nestjs/swagger';
import { CommentType } from 'src/api.dto';

@Controller('comments')
export class CommentsController {
  constructor(private readonly commentsService: CommentsService) {}


  /**
   * Creates a comment
   * @param createCommentDto The data required to create the comment
   */
  @ApiBearerAuth()
  @ApiResponse({status: 200, description: "Successfully created", type: CommentType})
  @ApiResponse({status: 401, description: "Invalid token"})

  @UseGuards(JwtAuthGuard)
  @HttpCode(201)
  @Post()
  create(@Body() createCommentDto: CreateCommentDto, @Req() req: any) {
    return this.commentsService.create(createCommentDto, req.user.id);
  }

  /**
   * Returns all comments on a post
   * @param id The id of the post
   * @returns Array of comments on the post
   */
  @ApiResponse({status: 200, description: "Returns all the comments on the post", type: [CommentType]})
  @ApiResponse({status: 404, description: "Post with the specified id doesn't exist"})
  @ApiParam({name: "id", description: "The id of the post"})

  @Get('post/:id')
  findAllOnPost(@Param('id', ParseIntPipe) id: number) {
    return this.commentsService.findAllOnPost(id);
  }

  /**
   * Returns a specific comment
   * @param id The id of the comment
   * @returns The comment
   */
  @ApiResponse({status: 200, description: "Returns the commnet", type: CommentType})
  @ApiResponse({status: 404, description: "Comment not found"})
  @ApiParam({name: "id", description: "The id of the comment"})

  @Get(':id')
  findOne(@Param('id', ParseIntPipe) id: number) {
    return this.commentsService.findOne(id);
  }

  /**
   * Upadtes a comment
   * @param id The id of the comment
   * @param updateCommentDto The data to change
   * @returns The updated comment
   */
  @ApiBearerAuth()
  @ApiResponse({status: 200, description: "Comment succsesfully updated", type: CommentType})
  @ApiResponse({status: 401, description: "Invalid token"})
  @ApiResponse({status: 404, description: "Comment not found, or the user can't edit it"})
  @ApiParam({name: "id", description: "The id of the comment"})

  @UseGuards(JwtAuthGuard)
  @Patch(':id')
  update(@Param('id', ParseIntPipe) id: number, @Body() updateCommentDto: UpdateCommentDto, @Req() req: any) {
    return this.commentsService.update(id, updateCommentDto, req.user.id);
  }

  /**
   * Deletes a comment
   * @param id The id of the comment
   * @returns 
   */
  @ApiResponse({status: 204, description: "Comment successfully deleted"})
  @ApiResponse({status: 401, description: "Invalid token"})
  @ApiResponse({status: 404, description: "Comment not found, or the user can't delete it"})
  @ApiParam({name: "id", description: "The id of the comment"})
  @ApiBearerAuth()

  @UseGuards(JwtAuthGuard)
  @HttpCode(204)
  @Delete(':id')
  async remove(@Param('id', ParseIntPipe) id: number, @Req() req: any) {
    await this.commentsService.remove(id, req.user.id);
  }
}
