import { io, server, app } from "./sockets";
// import express from "express";
// import router from "./router";
const port = 3000;

server.listen(port, () =>
  console.log(`App listening at http://localhost:${port}`)
);

export default { app, server };
