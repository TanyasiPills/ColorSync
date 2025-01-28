import { Socket } from "socket.io";
import { User } from "./types";

export function socketError(socket: Socket, message: string, statusCode: number, disconnect: boolean = true) {
  socket.emit('error', {message, statusCode});
  if (disconnect) socket.disconnect(true);
}

export function checkUser(socket: Socket): User {
  const user: User = socket.data.user;
  if (!user) {
    socketError(socket, 'You are not logged in', 10, false);
    return null;
  }
  return user;
}
