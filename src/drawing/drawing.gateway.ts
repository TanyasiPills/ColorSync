import { Logger } from "@nestjs/common";
import {
  ConnectedSocket,
  MessageBody,
  OnGatewayConnection,
  OnGatewayDisconnect,
  OnGatewayInit,
  SubscribeMessage,
  WebSocketGateway,
  WebSocketServer
} from "@nestjs/websockets";

import { Server, Socket } from "socket.io";
import { AuthService } from "src/auth/auth.service";
import { Room } from "./room";
import { checkUser, socketError } from "./error";
import { User } from "./types";
@WebSocketGateway({ cors: { origin: "*" } })

export class DrawingWS
  implements OnGatewayInit, OnGatewayConnection, OnGatewayDisconnect {
  constructor (private readonly authService: AuthService) {}
  private readonly logger = new Logger(DrawingWS.name);
  private rooms: Room[];
  private connections: Map<string, Room>;
  private connectedUsers: number[];

  @WebSocketServer() server: Server;

  afterInit() {
    Room.init(this.server);
    this.rooms = [];
    this.connections = new Map<string, Room>;
    this.connectedUsers = [];
  }

  async handleConnection(socket: Socket) {
    const token = socket.handshake.headers.token as string;
    let password = socket.handshake.headers.password as string;
    let name = socket.handshake.query.name;

    if (password === "") password = undefined;

    console.log("Token: ", token);
    console.log("Password: ", password);

    if (!name) {
      socketError(socket, "Name is required", 20);
      return;
    }
    if (Array.isArray(name)) name = name[0];

    if (!token) {
      socketError(socket, "Token is required", 20);
      return;
    }

    const user = await this.authService.validateToken(token);
    if (!user) {
      socketError(socket, "Invalid token", 30);
      return;
    }
    delete user.password;

    if (this.connectedUsers.includes(user.id)) {
      socketError(socket, "User is already connected", 40);
      return;
    }
    
    socket.data.user = user;

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
      room = new Room(name, password, Math.min(maxClients, 10), socket, user);
      this.rooms.push(room);
    } else {
      room = this.rooms.find(room => room.getName() === name);
      if (!room) {
        socketError(socket, "Room not found", 50);
        return;
      }
      if (!room.connect(socket, password)) {
        socket.disconnect();
        return;
      }
    }
    
    this.connections.set(socket.id, room);
    this.connectedUsers.push(user.id);
    this.logger.log(`${user.username} connected to room "${name}"`);
  }

  handleDisconnect(socket: Socket) {
    this.connectedUsers = this.connectedUsers.filter(id => id !== socket.data.user.id);
    const user = socket.data.user as User;
    if (!user) return;
    const room = this.connections.get(socket.id);
    if (room) room.disconnect(socket);
    this.logger.log(`${user.username} disconnected`);
  }

  @SubscribeMessage('message')
  handleMessage(@ConnectedSocket() socket: Socket, @MessageBody('message') message: string) {
    const user = checkUser(socket);
    if (!user) return;
    const room = this.connections.get(socket.id);
    room.emitFromSocket(socket, 'message', {userId: user.id, username: user.username, message: message});
  }

  @SubscribeMessage('mouse')
  handleMouse(@ConnectedSocket() socket: Socket, @MessageBody('position') position: {x: number, y: number}) {
    const user = checkUser(socket);
    if (!user) return;
    const room = this.connections.get(socket.id);
    room.emitFromSocket(socket, 'mouse', {userId: user.id, position: position});
  }

  @SubscribeMessage('action')
  handleAction(@ConnectedSocket() socket: Socket, @MessageBody('type') type: string, @MessageBody('data') data: any) {
    const user = checkUser(socket);
    if (!user) return;
    const room = this.connections.get(socket.id);
    room.emitFromSocket(socket, 'action', {type, data});
  }
}
