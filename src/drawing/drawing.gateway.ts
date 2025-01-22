import { Logger } from "@nestjs/common";
import {
  OnGatewayConnection,
  OnGatewayDisconnect,
  OnGatewayInit,
  WebSocketGateway,
  WebSocketServer
} from "@nestjs/websockets";

import { Server, Socket } from "socket.io";

@WebSocketGateway({ cors: { origin: "*" } })

export class DrawingWS
  implements OnGatewayInit, OnGatewayConnection, OnGatewayDisconnect {
  private readonly logger = new Logger(DrawingWS.name);
  //private rooms: Map<string, {password: string | null, history: Map<string, {history: { action: string, data: string }[], undo: { action: string, data: string }[]}>}> = new Map();

  @WebSocketServer() server: Server;

  afterInit() {
    this.logger.log("Initialized");
  }

  handleConnection(client: Socket) {
    this.logger.log(`Client id: ${client.id} connected`);
  }

  handleDisconnect(client: any) {
    this.logger.log(`Cliend id: ${client.id} disconnected`);
  }

  /*

  @SubscribeMessage('message')
  handleMessage(@ConnectedSocket() client: Socket, @MessageBody('text') text: string) {
    this.logger.log(`Message received from client id: ${client.id}`);
    this.logger.debug(`Payload: ${text}`);
    this.server.in(Array.from(client.rooms)[1]).emit('message', { text: text });
  }

  @SubscribeMessage('join')
  handleJoin(@ConnectedSocket() client: Socket, @MessageBody('room') roomName: string, @MessageBody('password') password: string) {
    const room = this.rooms.get(roomName);
    if (!room) return;
    if (room.password) {
      if (!password) {
        client.emit('error', { text: 'password required' });
        return;
      }
      if (room.password != password) {
        client.emit('error', { text: 'wrong password' });
        return;
      }
    }
    this.logger.log(`client (${client.id}) joined ${roomName}`)
    this.server.in(client.id).socketsJoin(roomName);
    this.server.in(roomName).emit('message', `${client.id} joined the room`);
    client.emit('message', Object.fromEntries(room.history.entries()));
  }

  @SubscribeMessage('create')
  handleCreate(@ConnectedSocket() client: Socket, @MessageBody('room') roomName: string, @MessageBody('password') password: string) {
    if (this.rooms.get(roomName)) {
      client.emit('error', 'room already exists');
      return;
    }
    this.logger.log(`client (${client.id}) created ${roomName}`)
    this.server.in(client.id).socketsJoin(roomName);
    const room = {password: password, history: new Map<string, {history: { action: string, data: string }[], undo: { action: string, data: string }[]}>};
    this.rooms.set(roomName, room);
    client.emit('log', 'room created');
  }

  @SubscribeMessage('drawAction')
  handleDrawAction(@ConnectedSocket() client: Socket, @MessageBody('action') action: string, @MessageBody('data') data: string) {
    const roomName: string = this.GetRoom(client);
    if (!roomName) {
      client.emit('error', 'You are not in any room!');
      return;
    }
    const roomPtr = this.rooms.get(roomName);
    const clientPtr = roomPtr.history.get(client.id);
    clientPtr.push({action, data}); //???

    console.log(this.rooms);
    this.server.in(roomName).emit('action', {action, data});
  }

  GetRoom(client: Socket): string {
    return Array.from(client.rooms)[1];
  }

  @SubscribeMessage('action')
  handelAction(@ConnectedSocket() client: Socket, @MessageBody('action') action: string, @MessageBody('data') data: string) {
    switch (action) {
      case 'undo':

    }
  }
  */
}
