import { Module } from '@nestjs/common';
import { PostsService } from './posts.service';
import { PostsController } from './posts.controller';
import { PrismaService } from 'src/prisma.service';
import { SearchModule } from 'src/search/search.module';

@Module({
  controllers: [PostsController],
  providers: [PostsService, PrismaService],
  imports: [SearchModule]
})
export class PostsModule {}
