import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { AppService } from './app.service';
import { DrawingWS } from './drawing/drawing.gateway';
import { AuthModule } from './auth/auth.module';
import { UsersModule } from './users/users.module';
import { DrawingModule } from './drawing/drawing.module';
import { PostsModule } from './posts/posts.module';
import { ImagesModule } from './images/images.module';
import { CommentsModule } from './comments/comments.module';
import { SearchModule } from './search/search.module';
import { PrismaService } from './prisma.service';

@Module({
  imports: [AuthModule, UsersModule, DrawingModule, PostsModule, ImagesModule, CommentsModule, SearchModule],
  controllers: [AppController],
  providers: [AppService, DrawingWS, PrismaService]
})
export class AppModule {}
