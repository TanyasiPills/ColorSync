import { ApiProperty } from "@nestjs/swagger";
import { Visibility } from "@prisma/client";
import { Transform } from "class-transformer";
import { IsEnum, IsOptional } from "class-validator";

export class UpdateImageDto {
    @IsOptional()
    @IsEnum(Visibility)
    @ApiProperty({ description: 'The visibility of the image', examples: ['public', 'private'], enum: Visibility })
    visibility?: Visibility;

    @IsOptional()
    @ApiProperty({ description: 'Force the update' })
    forceUpdate?: boolean;
}
