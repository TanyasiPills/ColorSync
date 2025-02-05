import { IsInt, IsOptional, IsString } from "class-validator";

export class CreatePostDto {
  @IsOptional()
  @IsInt()
  imageId: number | null;

  @IsString()
  text: string;
}
