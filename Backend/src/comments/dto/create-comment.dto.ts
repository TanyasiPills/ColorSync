import { IsInt, IsString, MaxLength, Min, MinLength } from "class-validator";

export class CreateCommentDto {
    @IsInt()
    @Min(1)
    postId: number;

    @IsString()
    @MinLength(1)
    @MaxLength(500)
    text: string
}
