import { Module } from '@nestjs/common';
import { AuthModule } from 'src/auth/auth.module';

@Module({})
export class DrawingModule {
  imports: [AuthModule]
}
