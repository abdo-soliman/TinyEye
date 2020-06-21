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
    name: Sequelize.STRING,
    classId: Sequelize.INTEGER,
    boardId: {
      type: Sequelize.INTEGER,
      references: {
        model: "boards",
        key: "id",
      },
    },
  },
  {
    timestamps: true,
  }
);

export default Humans;
