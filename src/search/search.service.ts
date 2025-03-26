import { Injectable, Logger, OnModuleInit } from '@nestjs/common';
import { ElasticsearchService } from '@nestjs/elasticsearch';
import { PrismaService } from 'src/prisma.service';

@Injectable()
export class SearchService implements OnModuleInit {
  constructor(
    private readonly elastic: ElasticsearchService,
    private readonly db: PrismaService
  ) { }
  private readonly logger = new Logger(SearchService.name);

  async onModuleInit() {
    //await this.createIndex();
    await this.reindexDatabase();
  }

  async indexPost(id: number, text: string, tags: string[], date: Date) {
    if (tags) tags = tags.map(e => e.toLowerCase());
    await this.elastic.index({
      index: 'post',
      id: id.toString(),
      body: {
        text,
        tags,
        date
      }
    });
  }

  async updatePost(id: number, text?: string, tags?: string[], date?: Date) {
    if (tags) tags = tags.map(e => e.toLowerCase());
    const result = await this.elastic.update({
      index: 'post',
      id: id.toString(),
      body: {
        doc: { text, tags, date }
      }
    });
  }

  async deletePost(id: number) {
    const result = await this.elastic.delete({
      index: 'post',
      id: id.toString()
    })
  };

  private async createIndex() {
    const indexExists = await this.elastic.indices.exists({ index: "post" });

    if (indexExists) {
      this.logger.log('Index "post" already exists')
    }
    this.logger.log("Creating index: post")
    await this.elastic.indices.create({
      index: "post",
      body: {
        settings: {
          number_of_shards: 1,
          number_of_replicas: 1
        }
      },
      mappings: {
        properties: {
          text: { type: 'text' },
          tags: { type: 'keyword' },
          date: { type: 'date' }
        }
      }
    });
  }

  async reindexDatabase() {
    this.logger.log('Reindexing database...')

    const indexExists = await this.elastic.indices.exists({ index: "post" });

    if (indexExists) {
      this.logger.log('Deleting index: post');
      await this.elastic.indices.delete({ index: 'post' });
    }

    await this.createIndex();

    this.logger.log('Indexing all posts...');
    const posts: any = await this.db.post.findMany({ select: { id: true, text: true, tags: true, date: true } });
    posts.map(e => e.tags = e.tags.map(e => e.name));
    await Promise.all(posts.map(post => this.indexPost(post.id, post.text, post.tags, post.date)));
    this.logger.log('Reindexing complete');
  }

  async searchPosts(searchText: string, searchTags: string[], offset: number = 0, take: number = 10, userId: number = undefined): Promise<number[]> {
    if (searchTags) searchTags = searchTags.map(e => e.toLowerCase());
    if (searchText == "") searchText == undefined;
    
    const userPreferences: { queries: { text: string, weight: number }[], tags: { name: string, weight: number }[] } = {queries: [], tags: []};

    const userPrefenceMultiplier = { text: 0, tags: 0 };
    if (userId) {
      const likes = await this.db.user.findUnique({where: {id: userId}, select: {likes: {select: {text: true, tags: true}, take: 100}}});
      let tags = new Map<string, number>();
      for (let e of likes.likes) { // e: {text: string, tags: {name: string}}
        userPreferences.queries.push({text: e.text, weight: 1});
        e.tags.forEach(tag => {
          if (tags.has(tag.name)) tags.set(tag.name, tags.get(tag.name));
          else tags.set(tag.name, 1);
        });
      }

      for (let kvp of tags.entries()) {
        userPreferences.tags.push({name: kvp[0], weight: kvp[1]});
      }

      if (userPreferences) {
        if (userPreferences.queries && userPreferences.queries.length > 0) {
          userPrefenceMultiplier.text = (0.5 / userPreferences.queries.length);
        }
        if (userPreferences.tags && userPreferences.tags.length > 0) {
          userPrefenceMultiplier.tags = (0.5 / userPreferences.tags.length);
        }
      }
    }

    //console.log(userPreferences);

    const result = await this.elastic.search({
      index: 'post',
      body: {
        query: {
          function_score: {
            query: {
              dis_max: {
                queries: [
                  ...(searchText ? [{
                    match: {
                      text: {
                        query: searchText,
                        boost: 2
                      }
                    }
                  }] : []),
                  ...((searchTags && searchTags.length > 0) ? searchTags.map(e => ({
                    term: { tags: e }
                  })) : []),
                  ...((!searchText && (!searchTags || searchTags.length == 0)) ? [{
                    match_all: {}
                  }] : [])
                ]
              }
            },
            functions: [
              ...(userPrefenceMultiplier.text > 0 ? userPreferences.queries.map(e => ({
                filter: {
                  match: {
                    text: e.text
                  }
                },
                weight: Math.max(userPrefenceMultiplier.text * e.weight, 0.01)
              })) : []),
              ...(userPrefenceMultiplier.tags > 0 ? userPreferences.tags.map(e => ({
                filter: {
                  term: {
                    tags: e.name
                  }
                },
                weight: Math.max(userPrefenceMultiplier.tags * e.weight, 0.01)
              })) : []),
              {
                exp: {
                  date: {
                    origin: 'now',
                    scale: '14d',
                    offset: '1d',
                    decay: 0.5
                  }
                }
              }
            ],
            score_mode: 'sum'
          }
        }
      },
      sort: [
        { _score: { order: 'desc' } },
        { date: { order: 'desc' } }
      ],
      from: offset,
      size: take
    });

    //console.log(result.hits.hits.map(e => ({id: e._id, score: e._score, text: (e._source as any).text, tags: (e._source as any).tags, date: (e._source as any).date })));

    return result.hits.hits.map(hit => parseInt(hit._id));
  }
}
