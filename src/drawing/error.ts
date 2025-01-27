import { Socket } from "socket.io";
import { User } from "./types";

export function checkUser(socket: Socket): User {
  const user: User = socket.data.user;
  if (!user) {
    socket.emit('error', {message: "You are not logged in!"});
    return null;
  }
  return user;
}
