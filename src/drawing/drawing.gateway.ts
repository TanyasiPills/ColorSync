import { Logger } from "@nestjs/common";
import {
  OnGatewayConnection,
  OnGatewayDisconnect,
  OnGatewayInit,
  WebSocketGateway,
  WebSocketServer
} from "@nestjs/websockets";

import { Server, Socket } from "socket.io";
import { AuthService } from "src/auth/auth.service";
import { Room } from "./room";
import { Client } from "./client";

@WebSocketGateway({ cors: { origin: "*" } })

export class DrawingWS
  implements OnGatewayInit, OnGatewayConnection, OnGatewayDisconnect {
  constructor (private readonly authService: AuthService) {}
  private readonly logger = new Logger(DrawingWS.name);
  private rooms: Room[] = [];
  private connections: Map<Client, Room> = new Map<Client, Room>;
  private clients: Map<string, Client> = new Map<string, Client>;

  @WebSocketServer() server: Server;

  afterInit() {
    this.logger.log("Initialized");
    Room.init(this.server);
  }

  async handleConnection(socket: Socket) {
    const token = socket.handshake.auth.token;
    const password = socket.handshake.auth.password;
    let name = socket.handshake.query.name;

    if (!name) {
      socket.emit('error', {message: 'Name is required!'});
      socket.disconnect();
      return;
    }
    if (Array.isArray(name)) name = name[0];

    if (!token) {
      socket.emit('error', {message: 'Token is required!'});
      socket.disconnect();
      return;
    } 
    const user = await this.authService.validateToken(token);
    if (!user) {
      socket.emit('error', {message: 'Invalid token!'});
      socket.disconnect();
      return;
    }

    const client = new Client(user, socket);
    const create = socket.handshake.query.create;
    let room: Room;
    if (create && create === 'true') {
      let maxClientsQuery = socket.handshake.query.maxClients;
      let maxClients = 4;
      if (maxClientsQuery) {
        try {
          if (Array.isArray(maxClientsQuery)) maxClientsQuery = maxClientsQuery[0]; 
          maxClients = parseInt(maxClientsQuery);
        } catch {}
      }
      room = new Room(name, password, Math.min(maxClients, 10), client);
      this.rooms.push(room);
    } else {
      room = this.rooms.find(room => room.getName() === name);
      if (!room) {
        socket.emit('error', {message: 'Room not found!'});
        socket.disconnect();
        return;
      }
      if (!room.connect(client, password)) {
        socket.disconnect();
        return;
      }
    }
    
    this.connections.set(client, room);
    this.clients.set(socket.id, client);
    this.logger.log(`Connected client id: ${socket.id}\nroom: ${name}\ntoken: ${token}\npassword: ${password}`);
  }

  handleDisconnect(socket: Socket) {
    const client = this.clients.get(socket.id);
    const room = this.connections.get(client);
    if (room) room.disconnect(client);
    this.logger.log(`Cliend id: ${client.getUsername()} disconnected`);
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
