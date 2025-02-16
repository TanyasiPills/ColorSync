import { ApiProperty } from "@nestjs/swagger";
import { IsInt, IsOptional, IsString, MaxLength, Min, MinLength } from "class-validator";

export class CreatePostDto {
  @IsOptional()
  @IsInt()
  @Min(0)
  @ApiProperty({ type: 'integer', description: 'The id of the image', example: 1 })
  imageId?: number;

  @IsString()
  @MinLength(1)
  @MaxLength(500)
  @ApiProperty({ type: 'string', description: 'The text of the post', example: 'Hello!' })
  text: string;
}
