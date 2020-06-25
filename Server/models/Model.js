import db from "./db";
import Sequelize from "sequelize";

const Models = db.define(
  "model",
  {
    id: {
      type: Sequelize.INTEGER,
      primaryKey: true,
      autoIncrement: true,
    },
    createdAt: {
      type: Sequelize.DATE,
    },
    updatedAt: {
      type: Sequelize.DATE,
    },
    mPath: Sequelize.STRING,
    mUrl: Sequelize.STRING,
    boardId: {
      type: Sequelize.INTEGER,
      references: {
        model: "boards",
        key: "id",
      },
      onUpdate: "cascade",
      onDelete: "cascade",
    },
  }
);

export default Models;
