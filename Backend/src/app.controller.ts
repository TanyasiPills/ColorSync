import { Controller, Get, Query, Req, Res, UseGuards } from '@nestjs/common';
import { AppService } from './app.service';
import { ApiExcludeEndpoint, ApiResponse } from '@nestjs/swagger';
import { Response } from 'express';
import { resolve } from 'path';
import { DrawingWS } from './drawing/drawing.gateway';
import { RoomType } from './api.dto';

@Controller()
export class AppController {
  constructor(private readonly appService: AppService, private readonly drawingWS: DrawingWS) { }

  /**
   * The index
   */
  @ApiExcludeEndpoint()
  @Get()
  index() {
    return {
      message: 'ColorSync API'
    };
  }

  /**
   * Returns the avalibale rooms
   * @returns 
   */
  @ApiResponse({ status: 204, description: "Returns the avaliable rooms", type: [RoomType] })
  @Get('rooms')
  getRooms() {
    return this.drawingWS.getRooms();
  }

  /**
 * Seaches for rooms
 * @returns 
 */
  @ApiResponse({ status: 204, description: "Returns the avaliable rooms", type: [RoomType] })
  @Get('rooms/search')
  searchRooms(@Query('q') q: string) {
    q = q.toLowerCase();
    return this.drawingWS.getRooms().filter(e => e.name.toLowerCase().includes(q));
  }

  /**
   * The websocket documentation
   */
  @ApiExcludeEndpoint()
  @Get('docs/websocket')
  websocketDocs(@Res() res: Response) {
    res.sendFile(resolve('files/websocket.html'));
  }
}
