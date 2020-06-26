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

// io.use(async (socket, next) => {
//   let UUID = socket.handshake.query.UUID;
//   if (await isValid(UUID)) {
//     return next();
//   }
//   return next(new Error("authentication error"));
// });

io.on("connection", (socket) => {
  console.log("connection start");
  let keys = Object.keys(io.sockets.clients().connected);
  for (const key of keys) {
    console.log(io.sockets.clients().connected[key].handshake.query.token);
  }
  // console.log(socket.handshake.query);
  // client.on('event', data => { /* … */ });
  socket.on("disconnect", () => {
    console.log("connection closed");
    let keys = Object.keys(io.sockets.clients().connected);
    for (const key of keys) {
      console.log(io.sockets.clients().connected[key].handshake.query.token);
    }
  });
});

const dynamicNsp = io.of(/^\/dynamic-\d+$/).on("connect", (socket) => {
  const newNamespace = socket.nsp; // newNamespace.name === '/dynamic-101'
  console.log(newNamespace.name);
  console.log(socket.handshake.query);
  // broadcast to all clients in the given sub-namespace
  // newNamespace.emit("hello");
});

export { io, server, app };
