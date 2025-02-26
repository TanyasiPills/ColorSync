import { ApiProperty } from '@nestjs/swagger';
import { IsString, MinLength, MaxLength, IsArray, IsOptional, IsInt, Min } from 'class-validator';

export class UpdatePostDto {
  @IsOptional()
  @IsInt()
  @Min(1)
  @ApiProperty({ type: 'integer', description: 'The id of the image', example: 1 })
  imageId?: number;
  
  @IsOptional()
  @IsString()
  @MinLength(1)
  @MaxLength(500)
  @ApiProperty({ type: 'string', description: 'The text of the post', example: 'Hello!' })
  text: string;

  @IsArray()
  @IsString({each: true})
  @IsOptional()
  @ApiProperty({ type: 'array', description: 'The tags on the post', example: ['funny', 'cat', 'meme']})
  tags?: string[];
}
