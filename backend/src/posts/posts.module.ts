import { Module } from '@nestjs/common';
import { PostsService } from './posts.service';
import { PostsController } from './posts.controller';
import { PrismaService } from 'src/prisma.service';
import { SearchModule } from 'src/search/search.module';
import { ImagesModule } from 'src/images/images.module';

@Module({
  controllers: [PostsController],
  providers: [PostsService, PrismaService],
  imports: [SearchModule, ImagesModule],
  exports: [PostsService]
})
export class PostsModule {}
