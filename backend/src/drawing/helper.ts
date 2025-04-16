import { Socket } from "socket.io";
import { User } from "./types";
import { Room } from "./room";

export function socketError(socket: Socket, message: string, statusCode: number, disconnect: boolean = false) {
  socket.emit('error', {message, statusCode});
  if (disconnect) socket.disconnect(true);
}

export function checkUser(socket: Socket): User {
  const user: User = socket.data.user;
  if (!user) {
    socketError(socket, 'You are not logged in', 10);
    return null;
  }
  return user;
}

export function isPositiveInt(value: any) {
  if (Number.isNaN(value)) return false;
  if (Number.isInteger(value)) {
    if (value >= 0) return true;
  }
  return false;
}

export function isLargerThan0Int(value: any) {
  if (Number.isNaN(value)) return false;
  if (Number.isInteger(value)) {
    if (value > 0) return true;
  }
  return false;
}

export function getRoom(socket: Socket, connections: Map<string, Room>) {
  const room = connections.get(socket.id);
  if (!room) {
    socketError(socket, 'You are not in a room', 11);
    return null;
  }
  return room;
}

export function getUserData(socket: Socket, connections: Map<string, Room>): [User, Room] {
  const user = checkUser(socket);
  if (!user) return [null, null];
  const room = getRoom(socket, connections);
  if (!room) return [null, null];
  return [user, room];
}
