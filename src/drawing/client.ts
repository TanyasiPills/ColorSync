import { Socket } from "socket.io";
import { User } from "./entities/user.entity";

export class Client extends User {
  private socket: Socket;
  public constructor(user: {id: number, username: string, email: string, profile_picture: string}, socket: Socket) {
    super(user.id, user.username, user.email, user.profile_picture);
    this.socket = socket;
  }

  public getSocket(): Socket {
    return this.socket;
  }
}
