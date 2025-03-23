import { Server, Socket } from "socket.io";
import { Action, History, User } from "./types";
import { checkUser, socketError } from "./helper";

export class Room {
  private static server: Server;
  private name: string;
  private width: number;
  private height: number;
  private password: string | undefined;
  private clients: Socket[];
  private maxClients: number;
  private owner: Socket;
  private history: History;

  public constructor(name: string, password: string | undefined, maxClients: number, owner: Socket, ownerUser: User, width: number, height: number) {
    this.name = name;
    this.width = width;
    this.height = height;
    this.password = password;
    this.maxClients = maxClients;
    this.clients = [owner];
    this.owner = owner;
    this.history = new History(30);
    this.history.connect(ownerUser.id);
    owner.join(name);

    console.log("room width: ", width);
  }

  public static init(server: Server): void {
    Room.server = server;
  }

  public connect(socket: Socket, pasword: string | undefined): boolean {
    const user : User = checkUser(socket);
    if (!user) return;
    if (this.password && pasword !== this.password) {
      socketError(socket, "Incorrect password", 31, false);
      return false;
    }
    socket.join(this.name);
    socket.emit('system message', {
      type: 3,
      width: this.width,
      height: this.height,
      history: this.history.getActions(),
      compilePosition: this.history.getUndoPosition(),
      users: this.clients.map(e => e.data.user),
      owner: this.owner.data.user.id
    });
    this.clients.push(socket);
    this.history.connect(user.id);
    this.emitFromSocket(socket, 'system message', {type: 1, username: user.username, message: `${user.username} joined the room!`, id: user.id});
    return true;
  }

  public disconnect(socket: Socket): boolean {
    const user : User = socket.data.user as User;
    socket.leave(this.name);
    this.clients = this.clients.filter(c => c !== socket);
    this.history.disconnect(user.id);
    this.emit('system message', {type: 2, message: `${user.username} left the room!`, id: user.id});
    socket.disconnect();

    if (this.clients.length === 0) return true;
    else {
      if (this.owner === socket) {
        this.changeOwnerP(this.clients[0]);
      }
      return false;
    }
  }

  private changeOwnerP(socket: Socket) {
    const user = checkUser(socket);
    if (!user) return;
    const newOwner = this.clients.find(e => e.data.user.id == user.id);
    if (!newOwner) return;
    this.owner = newOwner;
    this.emit('system message', {type: 4, message: `${newOwner.data.user.username} is now the owner of the room!`, id: newOwner.data.user.id});
  }

  public changeOwner(socket: Socket, userId: number) {
    if (this.owner !== socket) {
      socketError(socket, 'Unauthorized', 32);
    }
    const newOwner = this.clients.find(e => e.data.user.id == userId);
    if (!newOwner) {
      socketError(socket, 'User not found', 51);
      return;
    }
    this.owner = newOwner;
    this.emit('system message', {type: 4, message: `${newOwner.data.user.username} is now the owner of the room!`, id: newOwner.data.user.id});
  }

  public emit(event: string, message: any): void {
    Room.server.in(this.name).emit(event, message);
  }

  public emitFromSocket( socket: Socket, event: string, message: any): void {
    socket.to(this.name).emit(event, message);
  }

  public action(socket: Socket, action: Action): void {
    const user = checkUser(socket);
    if (!user) return;
    this.history.add(user.id, action);
    this.emitFromSocket(socket, 'action', action);
  }

  public undo(socket: Socket): void {
     const user = checkUser(socket);
     if (!user) return;
     const undo = this.history.undo(user.id);
     if (undo) this.emitFromSocket(socket, 'action', {type: "undo", data: undo});
  }

  public redo(socket: Socket): void  {
    const user = checkUser(socket);
    if (!user) return;
    const action = this.history.redo(user.id);
    if (action) this.emitFromSocket(socket, 'action', action);
  }

  public kick(socket: Socket, id: number): boolean {
    if (socket !== this.owner) {
      socketError(socket, 'Unauthorized', 32);
      return false;
    }
    const client: Socket = this.clients.find(e => e.data.user.id == id);
    if (!client) {
      socketError(client, 'User not found', 51);
      return false;
    }
    client.emit('system message', {type: 0, message: 'You have been kicked from the room!'});
    this.disconnect(client);
    return true;
  }

  public getName(): string {
    return this.name;
  }

  public getPassword(): string | undefined {
    return this.password;
  }

  public getMaxClients(): number {
    return this.maxClients;
  }

  public getClients(): Socket[] {
    return this.clients;
  }

  public getOwner(): Socket {
    return this.owner;
  }
}
