import request from "supertest";
import { app } from "../src/sockets";
import Sequelize from "sequelize";
import Board from "../models/Board";
import Log from "../models/Log";
const env = process.env.NODE_ENV || "test";
const config = require("../config/config.json")[env];

describe("Log", () => {
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

  test("get board log -- loggedin user", async () => {
    var res = await request(app).post("/register").send({
      email: "test@test.com",
      name: "Test name",
      password: "123456789",
      board_uuid: "123456789",
    });
    const { accessToken } = res.body;
    res = await request(app)
      .get("/log")
      .set({ Authorization: `Bearer ${accessToken}` })
      .send();
    expect(res.statusCode).toEqual(200);
  });

  test("get board log -- logged out user", async () => {
    const res = await request(app).get("/log").send();
    expect(res.statusCode).toEqual(401);
  });

  test("get board log by id -- loggedin user", async () => {
    const board = await Board.findOne({ where: { UUID: "123456789" } });
    const log = await Log.create({
      iUrl: "https://www.google.com",
      type: "unknown",
      personName: "unknown",
      boardId: board.dataValues.id,
    });
    var res = await request(app).post("/login").send({
      email: "test@test.com",
      password: "123456789",
    });
    const { accessToken } = res.body;
    res = await request(app)
      .post("/log/show")
      .set({ Authorization: `Bearer ${accessToken}` })
      .send({ id: log.dataValues.id });
    expect(res.statusCode).toEqual(200);
  });

  test("get board log by id -- logged out user", async () => {
    const board = await Board.findOne({ where: { UUID: "123456789" } });
    const log = await Log.create({
      iUrl: "https://www.google.com",
      type: "unknown",
      personName: "unknown",
      boardId: board.dataValues.id,
    });
    const res = await request(app)
      .post("/log/show")
      .send({ id: log.dataValues.id });
    expect(res.statusCode).toEqual(401);
  });

  test("mark log as read -- loggedin user", async () => {
    const board = await Board.findOne({ where: { UUID: "123456789" } });
    const log = await Log.create({
      iUrl: "https://www.google.com",
      type: "unknown",
      personName: "unknown",
      boardId: board.dataValues.id,
    });
    var res = await request(app).post("/login").send({
      email: "test@test.com",
      password: "123456789",
    });
    const { accessToken } = res.body;
    res = await request(app)
      .post("/log/markasread")
      .set({ Authorization: `Bearer ${accessToken}` })
      .send({ id: log.dataValues.id });
    expect(res.statusCode).toEqual(200);
    expect(res.body).toHaveProperty("readAt");
    expect(res.body.readAt).not.toBe(null);
  });

  test("mark log as read -- logged out user", async () => {
    const board = await Board.findOne({ where: { UUID: "123456789" } });
    const log = await Log.create({
      iUrl: "https://www.google.com",
      type: "unknown",
      personName: "unknown",
      boardId: board.dataValues.id,
    });
    const res = await request(app)
      .post("/log/markasread")
      .send({ id: log.dataValues.id });
    expect(res.statusCode).toEqual(401);
  });

  test("delete log by id -- loggedin user", async () => {
    const board = await Board.findOne({ where: { UUID: "123456789" } });
    const log = await Log.create({
      iUrl: "https://www.google.com",
      type: "unknown",
      personName: "unknown",
      boardId: board.dataValues.id,
    });
    var res = await request(app).post("/login").send({
      email: "test@test.com",
      password: "123456789",
    });
    const { accessToken } = res.body;
    res = await request(app)
      .post("/log/delete")
      .set({ Authorization: `Bearer ${accessToken}` })
      .send({ id: log.dataValues.id });
    expect(res.statusCode).toEqual(200);
  });

  test("delete log by id -- logged out user", async () => {
    const board = await Board.findOne({ where: { UUID: "123456789" } });
    const log = await Log.create({
      iUrl: "https://www.google.com",
      type: "unknown",
      personName: "unknown",
      boardId: board.dataValues.id,
    });
    const res = await request(app)
      .post("/log/delete")
      .send({ id: log.dataValues.id });
    expect(res.statusCode).toEqual(401);
  });
});
