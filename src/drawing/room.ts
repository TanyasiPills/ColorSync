import { Server, Socket } from "socket.io";
import { Action, ActionList, User } from "./types";
import { checkUser } from "./error";

export class Room {
  private static server: Server;
  private name: string;
  private password: string | undefined;
  private clients: Socket[];
  private maxClients: number;
  private owner: Socket;
  private history: {userId: number, action: Action}[];
  private redo: 

  public constructor(name: string, password: string | undefined, maxClients: number, owner: Socket) {
    this.name = name;
    this.password = password;
    this.maxClients = maxClients;
    this.clients = [owner];
    this.history = new Map<string, {undo: ActionList<Action>, redo: ActionList<Action>}>;
    this.owner = owner;
    owner.join(name);
    this.history.set(owner.id, {undo: new ActionList<Action>(30), redo: new ActionList<Action>(30)});
  }

  public static init(server: Server) {
    Room.server = server;
  }

  public connect(socket: Socket, pasword: string | undefined) {
    const user : User = checkUser(socket);
    if (!user) return;
    if (this.password && pasword !== this.password) {
      socket.emit('error', {message: 'Invalid password!'});
      return false;
    }
    socket.join(this.name);
    this.clients.push(socket);
    this.history.set(socket.id, {undo: new ActionList<Action>(30), redo: new ActionList<Action>(30)});
    this.emit('message', `${user.username} joined the room!`);
    return true;
  }

  public disconnect(socket: Socket) {
    const user : User = socket.data.user as User;
    socket.leave(this.name);
    console.log(`Number of clients before disconnect: ${this.clients.length}`);
    this.clients = this.clients.filter(c => c !== socket);
    console.log(`Number of clients after disconnect: ${this.clients.length}`);
    this.emit('message', `${user.username} left the room!`);
    socket.disconnect();
  }

  public emit(event: string, message: string) {
    Room.server.in(this.name).emit(event, message);
  }

  public emitFromSocket(event: string, message: any, socket: Socket) {
    socket.to(this.name).emit(event, message);
  }

  public action(socket: Socket, action: Action) {
    const user = checkUser(socket);
    if (!user) return;
    const history = this.history.get(socket.id);
    history.undo.add(action);
    this.emitFromSocket('action', action, socket);
  }

  public undo(socket: Socket) {
    const user =
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
