import express from "express";
import socketio from "socket.io";
const app = express();
import http from "http";
import Board from "../../models/Board";
const server = http.createServer(app);

const io = socketio(server);

const isValid = async (UUID) => {
  return await Board.findOne({ where: { UUID: UUID } });
};

io.use(async (socket, next) => {
  let UUID = socket.handshake.query.UUID;
  if (await isValid(UUID)) {
    return next();
  }
  return next(new Error("authentication error"));
});

io.on("connection", (socket) => {
  console.log("connection start");
  // console.log(socket.handshake.query);
  // client.on('event', data => { /* … */ });
  socket.on("disconnect", () => {
    console.log("connection closed");
  });
});

export { io, server, app };
