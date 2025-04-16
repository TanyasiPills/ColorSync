import { ApiProperty } from "@nestjs/swagger";
import { Visibility } from "@prisma/client";
import { IsEnum, IsOptional } from "class-validator";

export class CreateImageDto {
    @IsOptional()
    @IsEnum(Visibility)
    @ApiProperty({ description: 'The visibility of the image', examples: ['public', 'private'], enum: Visibility })
    visibility?: Visibility;
}
