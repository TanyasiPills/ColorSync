import { ApiProperty } from "@nestjs/swagger";
import { Visibility } from "@prisma/client";
import { IsEmail, IsEnum, IsInt, IsOptional, IsPositive, IsString, MaxLength, MinLength } from "class-validator";

export class LoginBody {
  @IsEmail()
  email: string;

  @MinLength(3)
  @MaxLength(24)
  @IsString()
  password: string;
}

export class UserInfo {
  @IsInt()
  @IsPositive()
  id: number;

  @IsString()
  @MinLength(3)
  @MaxLength(24)
  username: string
}

export class LoginResponse extends UserInfo {
  @IsString()
  access_token: string;
}

export class FileAPIType {
  @ApiProperty({ type: 'string', format: 'binary' })
  file: any;
}

export class ImageAPIType extends FileAPIType {
  @IsOptional()
  @IsEnum(Visibility)
  visibility?: Visibility;
}
