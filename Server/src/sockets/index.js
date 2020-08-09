import express from "express";
import socketio from "socket.io";
const app = express();
import http from "http";
import Board from "../../models/Board";
import ModelMan from "../modules/ModelMan";
import LogMan from "../modules/LogMan";
const server = http.createServer(app);

const io = socketio(server);

const isValid = async (UUID) => {
  return await Board.findOne({ where: { UUID: UUID } });
};

io.use(async (socket, next) => {
  let UUID = socket.handshake.query.serial;
  if (await isValid(UUID)) {
    return next();
  }
  return next(new Error("authentication error"));
});

io.on("connection", (socket) => {
  console.log("connection start");
  let keys = Object.keys(io.sockets.clients().connected);
  for (const key of keys) {
    console.log(io.sockets.clients().connected[key].handshake.query.serial);
  }
  // console.log(socket.handshake.query);
  // client.on('event', data => { /* … */ });
  socket.on("disconnect", () => {
    console.log("connection closed");
    let keys = Object.keys(io.sockets.clients().connected);
    for (const key of keys) {
      console.log(io.sockets.clients().connected[key].handshake.query.serial);
    }
  });
  socket.on("chat", (data) => {
    console.log(data);
  });
});

const boardNsp = io.of(/^\/board-\d+$/).on("connect", (socket) => {
  const newNamespace = socket.nsp; // newNamespace.name === '/board-101'
  // console.log(newNamespace.name);
  // console.log(socket.handshake.query);
  socket.on("get_model", async () => {
    const UUID = socket.handshake.query.serial;
    const model = await ModelMan.fetchUpdatedModel(UUID);
    if (model) {
      socket.emit("model", {
        model: model.dataValues.mUrl,
        map: model.dataValues.mapUrl,
        updated: true,
      });
    }
  });

  socket.on("log", async (data) => {
    const logInfo = JSON.parse(data);
    const boardId = socket.handshake.query.serial;
    LogMan.log(logInfo, boardId);
  });
  // broadcast to all clients in the given sub-namespace
  // newNamespace.emit("hello");
});

export { io, server, app };
