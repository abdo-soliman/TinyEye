import { io, server, app } from "./sockets";
import express from "express";
import router from "./router";
const port = 3000;

io.on("connection", (socket) => {
  // console.log("connection start");
  socket.on('chat', (msg) => {
    io.emit('chat', msg);
  });
  // console.log(socket.handshake.query);
  // client.on('event', data => { /* … */ });
  socket.on("disconnect", () => {
    console.log("connection closed");
  });
});


app.use(express.urlencoded({ extended: false }));
app.use(express.json());
app.use(express.static("storage"));

app.use("/", router);

server.listen(port, () =>
  console.log(`App listening at http://localhost:${port}`)
);
