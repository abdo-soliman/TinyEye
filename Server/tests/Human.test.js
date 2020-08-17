import request from "supertest";
import { app } from "../src/sockets";
import Sequelize from "sequelize";
import Board from "../models/Board";
import Human from "../models/Human";
const env = process.env.NODE_ENV || "test";
const config = require("../config/config.json")[env];

describe("Human", () => {
  beforeAll(async () => {
    new Sequelize(config.database, config.username, config.password, {
      host: config.host,
      dialect: config.dialect,

      pool: {
        max: 5,
        min: 0,
        idle: 10000,
      },
    });
    await Board.create({
      UUID: "123456789",
    });
  });

  afterAll(async () => {
    await Board.destroy({ where: { UUID: "123456789" } });
    console.log("finished");
  });

  test("get board humans -- loggedin user", async () => {
    var res = await request(app).post("/register").send({
      email: "test@test.com",
      name: "Test name",
      password: "123456789",
      board_uuid: "123456789",
    });
    const { accessToken } = res.body;
    res = await request(app)
      .get("/humans")
      .set({ Authorization: `Bearer ${accessToken}` })
      .send();
    expect(res.statusCode).toEqual(200);
  });

  test("get board humans -- logged out user", async () => {
    const res = await request(app).get("/humans").send();
    expect(res.statusCode).toEqual(401);
  });

  test("get board human by id -- loggedin user", async () => {
    const board = await Board.findOne({ where: { UUID: "123456789" } });
    const human = await Human.create({
      name: "test",
      dirPath: "/home/test",
      boardId: board.dataValues.id,
    });
    var res = await request(app).post("/login").send({
      email: "test@test.com",
      password: "123456789",
    });
    const { accessToken } = res.body;
    res = await request(app)
      .post("/humans/show")
      .set({ Authorization: `Bearer ${accessToken}` })
      .send({ id: human.dataValues.id });
    expect(res.statusCode).toEqual(200);
  });

  test("get board human by id -- logged out user", async () => {
    const board = await Board.findOne({ where: { UUID: "123456789" } });
    const human = await Human.create({
      name: "test",
      dirPath: "/home/test",
      boardId: board.dataValues.id,
    });
    const res = await request(app)
      .post("/humans/show")
      .send({ id: human.dataValues.id });
    expect(res.statusCode).toEqual(401);
  });
});
