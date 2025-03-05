import { ApiProperty } from "@nestjs/swagger";
import { Transform } from "class-transformer";
import { IsArray, IsInt, IsOptional, isString, IsString, MaxLength, Min, MinLength } from "class-validator";

export class CreatePostDto {
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

  @Transform(({ value }) => {
    try {
      console.log(isString(value));
      console.log(value);
      console.log(JSON.parse(value));
      return isString(value) ? JSON.parse(value) : value;
    } catch {
      return value;
    }
  })
  @IsArray()
  @IsString({each: true})
  @IsOptional()
  @ApiProperty({ type: 'array', description: 'The tags on the post', example: ['funny', 'cat', 'meme']})
  tags?: string[];
}
