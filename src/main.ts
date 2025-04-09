import { NestFactory } from '@nestjs/core';
import { AppModule } from './app.module';
import { NestExpressApplication } from '@nestjs/platform-express';
import { ValidationPipe } from '@nestjs/common';
import { DocumentBuilder, SwaggerModule } from '@nestjs/swagger';

async function bootstrap() {
  const app = await NestFactory.create<NestExpressApplication>(AppModule);
  
  app.enableCors({
    origin: '*',
    methods: 'GET,HEAD,PUT,PATCH,POST,DELETE,OPTIONS',
    preflightContinue: false,
    optionsSuccessStatus: 204,
  });

  app.useGlobalPipes(new ValidationPipe({transform: true, whitelist: true}));

  const docConfig = new DocumentBuilder()
    .setTitle("ColorSync")
    .addBearerAuth()
    .build();

  docConfig.tags.push({name: 'socket.io', description: 'WebSocket endpoints', externalDocs: {url: "/docs/websocket", description: "Websocket documentation."}});
  
  const document = SwaggerModule.createDocument(app, docConfig);
  SwaggerModule.setup('docs', app, document);
  
    
  await app.listen(3000);
}
bootstrap();
