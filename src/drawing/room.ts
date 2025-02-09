import { Server, Socket } from "socket.io";
import { Action, History, User } from "./types";
import { checkUser, socketError } from "./helper";

export class Room {
  private static server: Server;
  private name: string;
  private password: string | undefined;
  private clients: Socket[];
  private maxClients: number;
  private owner: Socket;
  private history: History;

  public constructor(name: string, password: string | undefined, maxClients: number, owner: Socket, ownerUser: User) {
    this.name = name;
    this.password = password;
    this.maxClients = maxClients;
    this.clients = [owner];
    this.owner = owner;
    this.history = new History(30);
    this.history.connect(ownerUser.id);
    owner.join(name);
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
      history: this.history.getActions(),
      compilePosition: this.history.getUndoPosition(),
      users: this.clients.map(e => e.data.user)
    });
    this.clients.push(socket);
    this.history.connect(user.id);
    this.emitFromSocket(socket, 'system message', {message: `${user.username} joined the room!`, id: user.id});
    return true;
  }

  public disconnect(socket: Socket): boolean {
    const user : User = socket.data.user as User;
    socket.leave(this.name);
    this.clients = this.clients.filter(c => c !== socket);
    this.history.disconnect(user.id);
    this.emit('system message', {message: `${user.username} left the room!`, id: user.id});
    socket.disconnect();

    if (this.clients.length === 0) return true;
    else return false;
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
    const client: Socket = this.clients.find(e => e.data.user.id == id);
    if (!socket) {
      socketError(socket, 'User not found', 51);
      return false;
    }
    this.disconnect(client);
    client.emit('system message', {message: 'You have been kicked from the room!'});
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
