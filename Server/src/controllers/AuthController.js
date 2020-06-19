import bcrypt from "bcrypt";
import Board from "../../models/Board";
import User from "../../models/User";
import jwt from "jsonwebtoken";

const JWT_SECRET = require("../../config/config.json")["JWT_SECRET"];

class AuthController {
  checkBoard = async (UUID, res) => {
    const board = await Board.findOne({
      where: { UUID: UUID },
    });
    return board;
  };

  checkEmail = async (email, res) => {
    const user = await User.findOne({
      where: { email: email },
    });
    return user;
  };

  register = async (req, res) => {
    const { body } = req;
    const board = await this.checkBoard(body.board_uuid, res);
    if (board == null) {
      return res
        .status(404)
        .json({ errors: [{ msg: "Board not found", param: "board_uuid" }] });
    }
    const databaseUser = await this.checkEmail(body.email, res);
    if (databaseUser) {
      return res
        .status(402)
        .json({ errors: [{ msg: "email already exists", param: "email" }] });
    }

    const hashedPassword = await bcrypt.hash(body.password, 10);
    const user = await User.create({
      Name: body.Name,
      email: body.email,
      password: hashedPassword,
      boardId: board.dataValues.id,
    });

    try {
      const accessToken = jwt.sign(user.dataValues, JWT_SECRET);
      return res.json({ accessToken: accessToken, user: user });
    } catch (error) {
      User.destroy({ where: { id: user.dataValues.id } });
      return res
        .status(500)
        .json({ errors: [{ msg: "Something went wrong!", param: "result" }] });
    }
  };

  login = async (req, res) => {
    const { body } = req;
    const user = await this.checkEmail(body.email, res);
    if (!user) {
      return res
        .status(404)
        .json({ errors: [{ msg: "User not found", param: "email" }] });
    }
    try {
      const accessToken = jwt.sign(user.dataValues, JWT_SECRET);
      return res.json({ accessToken: accessToken, user: user });
    } catch (error) {
      User.destroy({ where: { id: user.dataValues.id } });
      return res
        .status(500)
        .json({ errors: [{ msg: "Something went wrong!", param: "result" }] });
    }
  };
}

export default AuthController;
