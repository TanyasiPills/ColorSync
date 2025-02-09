import { IsInt, IsOptional, IsString, MaxLength, Min, MinLength } from "class-validator";

export class CreatePostDto {
  @IsOptional()
  @IsInt()
  @Min(0)
  imageId?: number;

  @IsString()
  @MinLength(1)
  @MaxLength(500)
  text: string;
}
