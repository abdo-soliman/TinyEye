import express from "express";
import router from "./router";
const app = express();
const port = 3000;

app.use(express.urlencoded({ extended: false }));
app.use(express.json());

app.use("/", router);

app.listen(port, () =>
  console.log(`App listening at http://localhost:${port}`)
);
