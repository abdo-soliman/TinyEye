import fs from "fs";
import pushNotification from "../notifications";
import User from "../../models/User";
import ServerLogger from "./ServerLogger";
import Log from "../../models/Log";
import Board from "../../models/Board";
const config = require("../../config/config.json");

class LogMan {
  static log = async (logInfo, UUID) => {
    const { image, type, person_name } = logInfo;
    const board = await Board.findOne({ where: { UUID: UUID } });
    if (board) {
      const boardId = board.dataValues.id;
      const now = Math.floor(new Date() / 1000);
      var myDirectory = `${__dirname}/../../storage/board_${boardId}`;
      fs.writeFile(
        `${myDirectory}/logs/images/${person_name}-${now}.jpg`,
        image,
        "base64",
        function (error) {
          if (error) {
            ServerLogger.error(error.message);
          }
        }
      );
      const imageUrl = `${config.url}/board_${boardId}/logs/images/${person_name}-${now}.jpg`;
      Log.create({
        iUrl: imageUrl,
        boardId: boardId,
        type: type,
        personName: person_name,
      });
      if (type == "unknown") {
        await this.sendNotification(boardId);
      }
    }
  };

  static getBoardUsers = async (boardId) => {
    const users = await User.findAll({ where: { boardId: boardId } });
    return users;
  };

  static sendNotification = async (boardId) => {
    const users = await this.getBoardUsers(boardId);
    const tokens = [];
    for (let index = 0; index < users.length; index++) {
      const user = users[index];
      tokens.push(user.dataValues.expoToken);
    }
    if (tokens !== []) {
      await pushNotification(
        tokens,
        "Unknown person",
        "Unknown person entered"
      );
    }
  };
}

export default LogMan;
