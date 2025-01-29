import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';
import { NestExpressApplication } from '@nestjs/platform-express';
import { ValidationPipe } from '@nestjs/common';
import { DocumentBuilder, SwaggerModule } from '@nestjs/swagger';
import { PathItemObject } from '@nestjs/swagger/dist/interfaces/open-api-spec.interface';

async function bootstrap() {
  const app = await NestFactory.create<NestExpressApplication>(AppModule);
  
  app.enableCors({
    origin: '*',
    methods: 'GET,HEAD,PUT,PATCH,POST,DELETE,OPTIONS',
    preflightContinue: false,
    optionsSuccessStatus: 204,
  });

  app.useGlobalPipes(new ValidationPipe());

  const docConfig = new DocumentBuilder()
    .setTitle("ColorSync")
    .addBearerAuth()
    .build();

  docConfig.tags.push({name: 'socket.io', description: 'WebSocket endpoints'});
    
  SwaggerModule.setup('docs', app, () => {
    const doc = SwaggerModule.createDocument(app, docConfig);
    console.log(doc.paths);
    let p : Record<string, PathItemObject> = {'/join/' : {get: {
      operationId: 'WebSocket_join',
      parameters: [],
      requestBody: null,
      responses: null,
      summary: 'Join a room',
      tags: ['socket.io']
    }}};
    doc.paths = p;
    return doc;
  });
    

  await app.listen(3000);
}
bootstrap();
