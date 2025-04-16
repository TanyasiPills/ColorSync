import { Module } from '@nestjs/common';
import { SearchService } from './search.service';
import { ElasticsearchModule } from '@nestjs/elasticsearch';
import { PrismaService } from 'src/prisma.service';

@Module({
  exports: [SearchService],
  imports: [ElasticsearchModule.register({
    node: 'http://localhost:9200',
    requestTimeout: 60000
  })],
  providers: [SearchService, PrismaService]
})
export class SearchModule { }
