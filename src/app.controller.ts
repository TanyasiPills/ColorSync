import { Controller, Get, Res } from '@nestjs/common';
import { AppService } from './app.service';
import { ApiExcludeController } from '@nestjs/swagger';
import { Response } from 'express';
import { resolve } from 'path';

@ApiExcludeController()
@Controller()
export class AppController {
  constructor(private readonly appService: AppService) {}

  @Get()
  index() {
    return {
      message: 'ColorSync API'
    };
  }

  @Get('docs/websocket')
  websocketDocs(@Res() res: Response) {
    res.sendFile(resolve('files/websocket.html'));
  }
}
