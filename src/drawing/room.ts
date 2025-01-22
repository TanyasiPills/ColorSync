import { Server } from "socket.io";
import { Client } from "./client";

export class Room {
  private static server: Server;
  private name: string;
  private password: string | undefined;
  private clients: Client[];
  private maxClients: number;
  private owner: Client;

  public constructor(name: string, password: string | undefined, maxClients: number, owner: Client) {
    this.name = name;
    this.password = password;
    this.maxClients = maxClients;
    this.clients = [owner];
    this.owner = owner;
    owner.getSocket().join(name);
  }

  public static init(server: Server) {
    Room.server = server;
  }

  public connect(client: Client, pasword: string | undefined) {
    if (this.password && pasword !== this.password) {
      client.getSocket().emit('error', {message: 'Invalid password!'});
      return false;
    }
    client.getSocket().join(this.name);
    this.clients.push(client);
    Room.server.in(this.name).emit('message', `${client.getUsername()} joined the room!`);
    return true;
  }

  public disconnect(client: Client) {
    client.getSocket().leave(this.name);
    this.clients = this.clients.filter(c => c !== client);
    Room.server.in(this.name).emit('message', `${client.getUsername()} left the room!`);
    client.getSocket().disconnect();
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

  public getClients(): Client[] {
    return this.clients;
  }

  public getOwner(): Client {
    return this.owner;
  }
}
