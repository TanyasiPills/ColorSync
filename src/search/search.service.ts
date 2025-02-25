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
        const indexExists = await this.elastic.indices.exists({ index: "post" });

        if (!indexExists) {
            this.createIndex();
        }

        this.reindexDatabase();
    }

    async indexPost(id: number, text: string, tags: string[], date: Date) {
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

    private async createIndex() {
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

        this.createIndex();

        this.logger.log('Indexing all posts...');
        const posts: any = await this.db.post.findMany({ select: { id: true, text: true, tags: true } });
        posts.map(e => e.tags = e.tags.map(e => e.name));
        Promise.all(posts.map(post => this.indexPost(post.id, post.text, post.tags, post.date))).then(() => this.logger.log('Reindexing complete'));
    }

    async searchPosts(searchText: string, searchTags: string[], userPreferences: { queries: { text: string, weight: number }[], tags: { name: string, weight: number }[] } = undefined, offset: number = 0, take: number = 10) {
        if (searchText == "") searchText == undefined;
        const userPrefenceMultiplier = { text: 0, tags: 0 };
        if (userPreferences) {
            if (userPreferences.queries && userPreferences.queries.length > 0) {
                userPrefenceMultiplier.text = 1 + (1 / userPreferences.queries.length);
            }
            if (userPreferences.tags && userPreferences.tags.length > 0) {
                userPrefenceMultiplier.tags = 1 + (1 / userPreferences.tags.length);
            }
        }

        console.log(userPreferences);
        console.log(userPrefenceMultiplier);

        const result = await this.elastic.search({
            index: 'post',
            body: {
                query: {
                    function_score: {
                        query: {
                            dis_max: {
                                queries: [
                                    {
                                        match: {
                                            text: {
                                                query: "eat",
                                                boost: 2
                                            }
                                        }
                                    },
                                    ...((searchTags && searchTags.length > 0) ? searchTags.map(e => ({
                                        term: { tags: e }
                                    })) : [])
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
                                weight: Math.max(userPrefenceMultiplier.text * e.weight, 1)
                            })) : []),
                            ...(userPrefenceMultiplier.tags > 0 ? userPreferences.tags.map(e => ({
                                filter: {
                                    term: {
                                        tags: e.name
                                    }
                                },
                                weight: Math.min(userPrefenceMultiplier.tags * e.weight)
                            })) : [])
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

        console.log(result.hits.hits.map(hit => ({ score: hit._score, id: hit._id, source: hit._source })));
    }
}
