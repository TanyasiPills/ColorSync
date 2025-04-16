import { ApiProperty, PartialType } from "@nestjs/swagger";
import { Visibility } from "@prisma/client";
import { Transform } from "class-transformer";
import { IsArray, IsBoolean, IsDate, IsEmail, IsEnum, IsInt, IsObject, IsOptional, IsPositive, IsString, MaxLength, Min, MinLength } from "class-validator";

export class LoginBodyType {
  @IsEmail()
  @ApiProperty({ type: 'string', description: 'The email of the user', example: 'example@example.com' })
  email: string;

  @MinLength(3)
  @MaxLength(24)
  @IsString()
  @ApiProperty({ type: 'string', description: 'The password of the user', example: 'secret password' })
password: string;
}

export class UserInfoType {
  @IsInt()
  @IsPositive()
  @ApiProperty({ type: 'integer', description: 'The id of the user', example: 1 })
  id: number;

  @IsString()
  @MinLength(3)
  @MaxLength(24)
  @ApiProperty({ type: 'string', description: 'The username of the user', example: 'bob' })
  username: string
}

export class UserInfoDescriptionType extends UserInfoType {
  @IsString()
  @ApiProperty({ type: 'string', description: 'The profile description of the user', example: 'Hello, I am Bob!' })
  profile_description: string;
}

export class UserInfoEmailType extends UserInfoType {
  @IsEmail()
  @ApiProperty({type: 'string', description: 'The email of the user', example: 'bob@example.com'})
  email: string;
}

export class UserInfoDescEmailType extends UserInfoDescriptionType {
  @IsEmail()
  @ApiProperty({type: 'string', description: 'The email of the user', example: 'bob@example.com'})
  email: string;
}

export class LoginResponseType extends UserInfoEmailType {
  @IsString()
  @ApiProperty({ type: 'string', description: 'The JWT token', example: "eyJhbGciOiJIUzI1NiIsInR5cKS6IkpXVCJ9.eyJ1c2VybmFtZSI6Im12YSIsInN1YiI6MSwiaWF0IjoxNzM5MDEwNzqwfQ.Ad-wyV-wtagv5vKZIVduMS977gUqIStLMT3RGoDAqAQ" })
  access_token: string;
}

export class FileType {
  @ApiProperty({ type: 'string', format: 'binary', description: "The image file" })
  file: any;
}

export class ImageCreateType extends FileType {
  @IsOptional()
  @IsEnum(Visibility)
  @ApiProperty({ description: 'The visibility of the image', examples: ['public', 'private'], enum: Visibility })
  visibility?: Visibility;
}

export class PostInputBodyType extends PartialType(FileType) {
  @IsOptional()
  @IsInt()
  @Min(1)
  @ApiProperty({ type: 'integer', description: 'The id of the image', example: 1 })
  imageId?: number;

  @IsString()
  @MinLength(1)
  @MaxLength(500)
  @ApiProperty({ type: 'string', description: 'The text of the post', example: 'Hello!' })
  text: string;

  @IsArray()
  @IsString({ each: true })
  @IsOptional()
  @ApiProperty({ type: 'array', description: 'The tags on the post', example: ['funny', 'cat', 'meme'] })
  tags?: string[];

  @IsOptional()
  @IsBoolean()
  @ApiProperty({ type: 'boolean', description: 'Change image to public if private, default = false', example: false })
  forcePost?: boolean;
}

export class PostBaseType {
  @IsInt()
  @IsPositive()
  @ApiProperty({ description: 'The id of the post', examples: [1, 2, 3] })
  id: number;

  @ApiProperty({ description: 'The text of the post', examples: ['Hello world!', 'This is a post'] })
  @IsString()
  text: string;

  @ApiProperty({ description: 'The date the post was created', examples: ['2021-01-01T00:00:00.000Z', '2025-01-08:01:10.252Z'] })
  @IsDate()
  date: Date;

  @ApiProperty({ description: 'The id of the user that created the post', examples: [1, 2, 3] })
  @IsInt()
  @IsPositive()
  userId: number;

  @ApiProperty({ description: 'The id of the image that is attached to the post', examples: [1, 2, 3] })
  @IsOptional()
  @IsInt()
  @IsPositive()
  imageId?: number;

  @ApiProperty({ description: 'The tags on the post', example: ['funny', 'cat', 'meme'] })
  @IsArray()
  @IsString({ each: true })
  tags: string[];

  @ApiProperty({ description: 'The number of likes on the post', example: 10 })
  @IsInt()
  @IsPositive()
  likes: number;
}

export class PostIncludesType {
  @IsInt()
  @IsPositive()
  @ApiProperty({ description: 'The id of the post', examples: [1, 2, 3] })
  id: number;

  @ApiProperty({ description: 'The text of the post', examples: ['Hello world!', 'This is a post'] })
  @IsString()
  text: string;

  @ApiProperty({ description: 'The date the post was created', examples: ['2021-01-01T00:00:00.000Z', '2025-01-08:01:10.252Z'] })
  @IsDate()
  date: Date;

  @ApiProperty({ description: 'The id of the image that is attached to the post', examples: [1, 2, 3] })
  @IsOptional()
  @IsInt()
  @IsPositive()
  imageId?: number;

  @ApiProperty({ description: 'The user that created the post', examples: [{ username: 'bob' }, { username: 'alice' }] })
  @IsObject()
  user: { username: string };

  @ApiProperty({ description: 'The tags on the post', example: ['funny', 'cat', 'meme'] })
  @IsArray()
  @IsString({ each: true })
  tags: string[];

  @ApiProperty({ description: 'The comments on the post', examples: [[{ id: 1, text: 'Hello, Bob', date: '2025-02-09T16:30:16.262Z', userId: 2, user: { username: 'William' } }], []] })
  @IsArray()
  comments: CommentType[];

  @ApiProperty({ description: 'The number of likes on the post', example: 10 })
  @IsInt()
  @IsPositive()
  likes: number;
}

export class CommentType {
  @IsInt()
  @IsPositive()
  @ApiProperty({ description: 'The id of the comment', examples: [1, 2, 3] })
  id: number;

  @IsString()
  @ApiProperty({ description: 'The text of the comment', examples: ['Hello world!', 'This is a comment'] })
  text: string;

  @IsDate()
  @ApiProperty({ description: 'The date the comment was created', examples: ['2021-01-01T00:00:00.000Z', '2025-01-08:01:10.252Z'] })
  date: Date;

  @IsObject()
  @ApiProperty({ description: 'The user that created the comment', examples: [{ id: 1, username: 'bob' }, { id: 2, username: 'alice' }] })
  user: UserInfoType;
}

export class ImageType {
  @IsInt()
  @IsPositive()
  @ApiProperty({ description: 'The id of the image', examples: [1, 2, 3] })
  id: number;

  @IsInt()
  @IsPositive()
  @ApiProperty({ description: 'The id of the user that created the image', examples: [1, 2, 3] })
  userId: number

  @IsDate()
  @ApiProperty({ description: 'The date the image was created', examples: ['2021-01-01T00:00:00.000Z', '2025-01-08:01:10:252Z'] })
  date: Date;

  @IsEnum(Visibility)
  @ApiProperty({ description: 'The visibility of the image', examples: ['public', 'private'], enum: Visibility })
  visibility: Visibility;
}

export class RoomType {
  @IsString()
  @ApiProperty({ description: 'The name of the room', examples: ['the best room', "bob's room"] })
  name: string;

  @IsObject()
  @ApiProperty({ description: 'The owner of the room', examples: [{ id: 1, username: 'bob' }, { id: 2, username: 'alice' }] })
  owner: UserInfoType;

  @IsInt()
  @Min(1)
  @ApiProperty({ description: 'The number of players inside the room', examples: [1, 2, 3, 4] })
  playerCount: number;

  @IsInt()
  @Min(1)
  @ApiProperty({ description: 'The player capacity of the room', examples: [4, 3, 2] })
  maxPlayers: number;

  @IsBoolean()
  @ApiProperty({ description: 'Does the room have a password', examples: [true, false] })
  passwordRequired: boolean;
}
