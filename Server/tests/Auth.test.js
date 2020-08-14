import request from "supertest";
import { app } from "../src/sockets";
import Sequelize from "sequelize";
import Board from "../models/Board";
const env = process.env.NODE_ENV || "test";
const config = require("../config/config.json")[env];

describe("Auth", () => {
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
  });

  afterAll(async () => {
    await Board.destroy({ where: { UUID: "123456789" } });
    console.log("finished");
  });

  test("register new user -- valid data", async () => {
    const board = await Board.create({ UUID: "123456789" });
    const res = await request(app).post("/register").send({
      email: "test@test.com",
      name: "Test name",
      password: "123456789",
      board_uuid: "123456789",
    });
    expect(res.statusCode).toEqual(200);
    expect(res.body).toHaveProperty("accessToken");
    expect(res.body).toHaveProperty("user");
  });

  test("register new user -- invalid email", async () => {
    const res = await request(app).post("/register").send({
      email: "test",
      name: "Test name",
      password: "123456789",
      board_uuid: "123456789",
    });
    expect(res.statusCode).toEqual(422);
    expect(res.body).toHaveProperty("errors");
  });

  test("register new user -- invalid board", async () => {
    const res = await request(app).post("/register").send({
      email: "test@test.com",
      name: "Test name",
      password: "123456789",
      board_uuid: "123456",
    });
    expect(res.statusCode).toEqual(404);
    expect(res.body).toHaveProperty("errors");
  });

  test("register new user -- existing email", async () => {
    const res = await request(app).post("/register").send({
      email: "test@test.com",
      name: "Test name",
      password: "123456789",
      board_uuid: "123456789",
    });
    expect(res.statusCode).toEqual(402);
    expect(res.body).toHaveProperty("errors");
  });

  test("login user -- valid data", async () => {
    const res = await request(app).post("/login").send({
      email: "test@test.com",
      password: "123456789",
    });
    expect(res.statusCode).toEqual(200);
    expect(res.body).toHaveProperty("accessToken");
    expect(res.body).toHaveProperty("user");
  });

  test("login user -- invalid email", async () => {
    const res = await request(app).post("/login").send({
      email: "test@test.com1",
      password: "123456789",
    });
    expect(res.statusCode).toEqual(422);
    expect(res.body).toHaveProperty("errors");
  });

  test("login user -- nonexisting email", async () => {
    const res = await request(app).post("/login").send({
      email: "test1@test.com",
      password: "123456789",
    });
    expect(res.statusCode).toEqual(404);
    expect(res.body).toHaveProperty("errors");
  });

  test("login user -- wrong password", async () => {
    const res = await request(app).post("/login").send({
      email: "test@test.com",
      password: "12345678",
    });
    expect(res.statusCode).toEqual(404);
    expect(res.body).toHaveProperty("errors");
  });

  test("get loggedin user -- valid data", async () => {
    var res = await request(app).post("/login").send({
      email: "test@test.com",
      password: "123456789",
    });
    const { accessToken } = res.body;
    res = await request(app)
      .get("/me")
      .set({ Authorization: `Bearer ${accessToken}` })
      .send();
    expect(res.statusCode).toEqual(200);
  });

  test("get loggedin user -- invalid token", async () => {
    const accessToken =
      "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZCI6MTgsIm5hbWUiOiJUZXN0IG5hbWUiLCJleHBvVG9rZW4iOm51bGwsImVtYWlsIjoidGVzdEB0ZXN0LmNvbSIsInBhc3N3b3JkIjoiJDJiJDEwJDVNYVFXWmI3dEhmOFgzQ0ZaRGNaT3VLZzVtZThpYkpraXkwZVRtUWhKYTNwdXlXUDU4Sk1HIiwiYm9hcmRJZCI6NDYsImNyZWF0ZWRBdCI6IjIwMjAtMDgtMTRUMTA6MTk6MDYuMDAwWiIsInVwZGF0ZWRBdCI6IjIwMjAtMDgtMTRUMTA6MTk6MDYuMDAwWiIsImlhdCI6MTU5NzQwMDM0N30.5URTBTGN8PiwnlXilxdEGWtVY9y1FeMzDvZ01eNVPO";
    const res = await request(app)
      .get("/me")
      .set({ Authorization: `Bearer ${accessToken}` })
      .send();
    expect(res.statusCode).toEqual(403);
  });

  test("get loggedin user -- no token", async () => {
    const res = await request(app).get("/me").send();
    expect(res.statusCode).toEqual(401);
  });
});
