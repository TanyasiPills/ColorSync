import { FileAPIType, LoginBody, LoginResponse, UserInfo } from './dto/api.dto';
import { Controller, Get, Post, Body, Patch, Param, Delete, UseGuards, Request, HttpCode, HttpException, HttpStatus, ParseIntPipe, UploadedFile, UseInterceptors, Res, NotFoundException, Req } from '@nestjs/common';
import { UsersService } from './users.service';
import { CreateUserDto } from './dto/create-user.dto';
import { UpdateUserDto } from './dto/update-user.dto';
import { LocalAuthGuard } from 'src/auth/local-auth.guard';
import { AuthService } from 'src/auth/auth.service';
import { JwtAuthGuard } from 'src/auth/jwt-auth.guard';
import { FileInterceptor } from '@nestjs/platform-express';
import { diskStorage } from 'multer';
import { extname } from 'path';
import { Response } from 'express';
import { ApiBearerAuth, ApiBody, ApiConsumes, ApiParam, ApiResponse } from '@nestjs/swagger';

@Controller('users')
export class UsersController {
  constructor(
    private readonly userService: UsersService,
    private readonly authService: AuthService
  ) {}


  /**
   * Login for the user
   * @returns {Promise<LoginResponse>} Login response
   */
  @ApiBody({type: LoginBody})
  @ApiResponse({status: 200, description: 'Login successful', type: LoginResponse})
  @ApiResponse({status: 401, description: 'Invalid credentials'})

  @UseGuards(LocalAuthGuard)
  @HttpCode(200)
  @Post('login')
  async login(@Request() req) {
    return this.authService.login(req.user);
  }


  /**
   * Creates a user, email must be unique
   * @param createUserDto The data to create the user
   * @returns 
   */
  @ApiResponse({status: 201, description: 'User created', type: LoginResponse})
  @ApiResponse({status: 409, description: 'Email already in use'})

  @Post()
  async create(@Body() createUserDto: CreateUserDto) {
    const status = await this.authService.register(createUserDto);
    if (status) return this.authService.login(status);
    else if (status == null) throw new HttpException('Email already in use.', HttpStatus.CONFLICT); 
    else throw new HttpException('Error creating user.', HttpStatus.INTERNAL_SERVER_ERROR);
  }

  /**
   * Upload a profile picture for the user
   * @param file image to upload
   */
  @ApiBearerAuth()
  @ApiConsumes('multipart/form-data')
  @ApiBody({description: 'Profile picture to upload', type: FileAPIType})
  @ApiResponse({status: 204, description: 'Profile picture uploaded'})
  @ApiResponse({status: 401, description: 'Invalid token'})

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
  @HttpCode(204)
  uploadPfp(@UploadedFile() file: Express.Multer.File, @Request() req) {
    if (!file) {
      throw new HttpException('File upload failed!.', HttpStatus.BAD_REQUEST);
    }

    this.userService.upload(file, req.user.id);
  }

  /**
   * Gets the profile picture of a specific user
   * @param id Id of the user
   * @returns Image file
   */
  @ApiParam({name: 'id', description: 'Id of the user'})
  @ApiResponse({status: 200, description: 'Returns the profile picture of the user'})
  @ApiResponse({status: 404, description: 'Image not found'})

  @Get(':id/pfp')
  async getPfp(@Param('id', ParseIntPipe) id: number, @Res() res: Response) {
    const path = await this.userService.getPfp(id);
    if (path) {
      res.sendFile(path);
    } else {
      throw new NotFoundException('Image not found')
    }
  }
  
  /**
   * Returns data about the currently logged in user
   * @returns {UserInfo} Information about the logged in user
   */
  @ApiBearerAuth()
  @ApiResponse({status: 200, description: 'Returns data about the currently logged in user', type: UserInfo})
  @ApiResponse({status: 401, description: 'Invalid token'})

  @UseGuards(JwtAuthGuard)
  @Get()
  getLoggedIn(@Request() req: any) {
    return this.userService.getLoggedIn(req.user.id);
  }
  
  /**
   * Returns data about a specific user
   * @param id Id of the user
   * @returns {UserInfo} Infromation about the user
   */
  @ApiParam({name: 'id', type: 'int', description: 'Id of the user', required: true})
  @ApiResponse({status: 200, description: 'Returns the user data', type: UserInfo})
  @ApiResponse({status: 404, description: 'User not found'})

  @Get(':id')
  async findOne(@Param('id', ParseIntPipe) id: number) {
    const user = await this.userService.findOne(id);
    if (user) return user;
    else throw new NotFoundException('User not found');
  }

  /**
   * Updates the user data
   * @param updateUserDto The data to update
   * @returns {UserInfo} User information after the update
   */
  @ApiBearerAuth()
  @ApiResponse({status: 200, description: 'Returns the updated user data', type: UserInfo})
  @ApiResponse({status: 401, description: 'Invalid token'})

  @UseGuards(JwtAuthGuard)
  @Patch()
  update(@Req() req : any, @Body() updateUserDto: UpdateUserDto) {
    return this.userService.update(req.user.id, updateUserDto);
  }

  /**
   * Delets the user
   */
  @ApiBearerAuth()
  @ApiResponse({status: 204, description: 'User deleted successfully'})
  @ApiResponse({status: 401, description: 'Invalid token'})
  @ApiResponse({status: 500, description: 'Failed to delete the user'})

  @UseGuards(JwtAuthGuard)
  @Delete()
  @HttpCode(204)
  remove(@Req() req: any) {
    const status = this.userService.remove(req.user.id);
    if (!status) throw new HttpException('Failed to delete the user', HttpStatus.INTERNAL_SERVER_ERROR)
  }
}
