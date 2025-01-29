import { ApiProperty } from "@nestjs/swagger";

export class LoginBody  {
  @ApiProperty()
  email: string;

  @ApiProperty()
  password: string;
}
