import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { AppService } from './app.service';
import { DrawingWS } from './drawing/drawing.gateway';
import { AuthModule } from './auth/auth.module';
import { UserModule } from './user/user.module';
import { DrawingModule } from './drawing/drawing.module';

@Module({
  imports: [AuthModule, UserModule, DrawingModule],
  controllers: [AppController],
  providers: [AppService, DrawingWS],
})
export class AppModule {}
