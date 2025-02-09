import { Visibility } from "@prisma/client";
import { IsEnum, IsOptional } from "class-validator";

export class CreateImageDto {
    @IsOptional()
    @IsEnum(Visibility)
    visibility?: Visibility;
}
