import db from "./db";
import Sequelize from "sequelize";

const Humans = db.define(
  "humans",
  {
    id: {
      type: Sequelize.INTEGER,
      primaryKey: true,
      autoIncrement: true,
    },
    Name: Sequelize.STRING,
    boardId: {
      type: Sequelize.INTEGER,
      references: {
        model: "boards",
        key: "id",
      }
    }
  }
);

export default Humans;
