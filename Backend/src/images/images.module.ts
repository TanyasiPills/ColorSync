import { Module } from '@nestjs/common';
import { ImagesService } from './images.service';
import { ImagesController } from './images.controller';
import { PrismaService } from 'src/prisma.service';
import { SearchModule } from 'src/search/search.module';

@Module({
  controllers: [ImagesController],
  imports: [SearchModule],
  providers: [ImagesService, PrismaService],
  exports: [ImagesService]
})
export class ImagesModule {}
