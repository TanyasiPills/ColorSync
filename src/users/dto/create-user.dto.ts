import { IsEmail, IsNotEmpty, IsString, MaxLength, MinLength } from "class-validator";

export class CreateUserDto {
	@IsNotEmpty()
	@IsString()
	@MinLength(3)
	@MaxLength(24)
	username: string;

	@IsNotEmpty()
	@IsEmail()
	email: string;

	@IsNotEmpty()
	@IsString()
	@MinLength(3)
	@MaxLength(24)
	password: string;
}
