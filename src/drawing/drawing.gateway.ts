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
import { getUserData, isLargerThan0Int, isPositiveInt, socketError } from "./helper";
import { User } from "./types";

@WebSocketGateway({ cors: { origin: "*" } })
export class DrawingWS
  implements OnGatewayInit, OnGatewayConnection, OnGatewayDisconnect {
  constructor(private readonly authService: AuthService) { }
  private readonly logger = new Logger(DrawingWS.name);
  private rooms: Room[];
  private connections: Map<string, Room>;
  private connectedUsers: number[];

  public getRooms() {
    return this.rooms.map(e =>({
      name: e.getName(),
      owner: e.getOwner().data.user,
      playerCount: e.getClients().length,
      maxPlayers: e.getMaxClients(),
      passwordRequired: e.getPassword() != undefined
    }));
  }

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

    if (!name) {
      socketError(socket, "Name is required", 20, true);
      return;
    }
    if (Array.isArray(name)) name = name[0];

    if (!token) {
      socketError(socket, "Token is required", 20, true);
      return;
    }

    const user = await this.authService.validateToken(token);
    if (!user) {
      socketError(socket, "Invalid token", 30, true);
      return;
    }

    delete user.password;
    delete user.email;
    delete user.profile_picture;

    socket.data.user = user;


    if (this.connectedUsers.includes(user.id)) {
      socketError(socket, "User is already connected", 40, true);
      return;
    }

    const create = socket.handshake.query.create;
    let room: Room;
    if (create && create === 'true') {
      if (this.rooms.some(e => e.getName().toLowerCase() === name)) {
        socketError(socket, "Name already in use", 41, true);
        return;
      }
      let maxClientsQuery = socket.handshake.query.maxClients;
      let maxClients = 4;
      if (maxClientsQuery) {
        try {
          if (Array.isArray(maxClientsQuery)) maxClientsQuery = maxClientsQuery[0];
          maxClients = parseInt(maxClientsQuery);
        } catch {
          socketError(socket, "Max clients must be a integer, defaulting to 4", 20);
        }
      }
      let widthQuery = socket.handshake.query.width;
      let heightQuery = socket.handshake.query.height;
      let width;
      let height;
      if (!widthQuery) {
        socketError(socket, "Width is required", 20, true);
        return;
      }
      if (!heightQuery) {
        socketError(socket, "Height is required", 20, true);
        return;
      }
      if (Array.isArray(widthQuery)) widthQuery = widthQuery[0];
      if (Array.isArray(heightQuery)) heightQuery = heightQuery[0];
      width = parseInt(widthQuery);
      height = parseInt(heightQuery);
      if (!isLargerThan0Int(width) || !isLargerThan0Int(height)) {
        socketError(socket, "Width and height must be positive integers", 20, true);
        return;
      }
    
      room = new Room(name, password, Math.min(maxClients, 10), socket, user, width, height);
      this.rooms.push(room);
    } else {
      room = this.rooms.find(room => room.getName() === name);
      if (!room) {
        socketError(socket, "Room not found", 50, true);
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
    const user = socket.data.user as User;
    if (!user) return;
    this.connectedUsers = this.connectedUsers.filter(id => id !== socket.data.user.id);
    const room = this.connections.get(socket.id);
    if (room) {
      const close = room.disconnect(socket);
      this.connections.delete(socket.id);
      if (close) {
        this.rooms = this.rooms.filter(e => e !== room);
      }
    }
    this.logger.log(`${user.username} disconnected`);
  }

  @SubscribeMessage('message')
  handleMessage(@ConnectedSocket() socket: Socket, @MessageBody('message') message: string) {
    const [user, room] = getUserData(socket, this.connections);
    if (!user) return;

    if (!message) {
      socketError(socket, 'Message is required', 20);
      return;
    }
    room.emitFromSocket(socket, 'message', { userId: user.id, username: user.username, message: message });
  }

  @SubscribeMessage('mouse')
  handleMouse(@ConnectedSocket() socket: Socket, @MessageBody('position') position: { x: number, y: number }) {
    const [user, room] = getUserData(socket, this.connections);
    if (!user) return;

    if (!position || !Number.isFinite(position.x) || !Number.isFinite(position.y)) {
      socketError(socket, 'Bad request format', 20);
      return;
    }
    room.emitFromSocket(socket, 'mouse', { userId: user.id, position: position });
  }

  @SubscribeMessage('action')
  handleAction(@ConnectedSocket() socket: Socket, @MessageBody('type') type: number, @MessageBody('data') data: any) {
    console.log("action\ntype", type);
    console.log("data", data);
    const [user, room] = getUserData(socket, this.connections);
    if (!user) return;

    if (!isPositiveInt(type)) {
      socketError(socket, 'Type must be a positive integer', 20);
      return;
    }
    switch (type) {
      case 0: // draw
        if (!data) {
          socketError(socket, 'Data is required', 20);
          return;
        }
        if (
          !isPositiveInt(data.layer) ||
          !isPositiveInt(data.brush) ||
          !data.color ||
          !Number.isFinite(data.color.r) ||
          !Number.isFinite(data.color.g) ||
          !Number.isFinite(data.color.b) ||
          !Number.isFinite(data.size) ||
          !data.offset ||
          !Number.isFinite(data.offset.x) ||
          !Number.isFinite(data.offset.y) ||
          !data.ratio ||
          !Number.isFinite(data.ratio.x) ||
          !Number.isFinite(data.ratio.y) ||
          !data.CurSca ||
          !Number.isFinite(data.CurSca.x) ||
          !Number.isFinite(data.CurSca.y) ||
          !Number.isFinite(data.CurSca.z) ||
          !Array.isArray(data.positions) ||
          !data.positions.every(e => Number.isFinite(e.x) && Number.isFinite(e.y))
        ) {
          socketError(socket, 'Bad data format', 20);
          return;
        }
        room.action(socket, { type, data });
        break;

      case 1: // add node
        if (!data) {
          socketError(socket, 'Data is required', 20);
          return;
        }
        if (
          !isPositiveInt(data.location) ||
          !isPositiveInt(data.node)
        ) {
          socketError(socket, 'Bad data format', 20);
          return;
        }
        room.action(socket, { type, data });
        break;

      case 2: // rename node
        if (!data) {
          socketError(socket, 'Data is required', 20);
          return;
        }
        if (
          !data.name ||
          !isPositiveInt(data.location)
        ) {
          socketError(socket, 'Bad data format', 20);
          return;
        }
        room.action(socket, { type, data });
        break;

      case 3: // undo
        room.undo(socket);
        break;

      case 4: // redo
        room.redo(socket);
        break;

      case 5: // delete node
        if (!data) {
          socketError(socket, 'Data is required', 20);
          return;
        }
        if (isPositiveInt(data.location)) {
          socketError(socket, 'Location must be a positive integer', 20);
          return;
        }
        room.action(socket, { type, data });
        break;

      case 6: // move node
        if (!data) {
          socketError(socket, 'Data is required', 20);
          return;
        }
        if (
          !isPositiveInt(data.node) ||
          !isPositiveInt(data.to)
        ) {
          socketError(socket, 'Bad data format', 20);
          return;
        }
        room.action(socket, { type, data });
        break;

      default:
        socketError(socket, 'Unknown type', 20);
        return;
    }
  }

  @SubscribeMessage('manage')
  handleManage(@ConnectedSocket() socket: Socket, @MessageBody('type') type: string, @MessageBody('data') data: any) {
    const [user, room] = getUserData(socket, this.connections);
    if (!user) return;

    switch (type) {
      case 'kick':
        if (!data) {
          socketError(socket, 'Data is requierd', 20);
          return;
        }
        if (!isPositiveInt(data.id)) {
          socketError(socket, 'Id must be a positive integer', 20);
          return;
        }
        room.kick(socket, data.id);
      case 'changeOwner':
        if (!data) {
          socketError(socket, 'Data is required', 20);
          return;
        }
        if (!isPositiveInt(data.id)) {
          socketError(socket, 'Id must be a positive integer', 20);
          return;
        }
        room.changeOwner(socket, data.id);
        break;
    }
  }
}
