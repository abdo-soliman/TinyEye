import db from "./db";
import Sequelize from "sequelize";

const Log = db.define(
  "logs",
  {
    iUrl: Sequelize.STRING,
    type: Sequelize.STRING,
    readAt: Sequelize.DATE,
    personName: Sequelize.STRING,
    boardId: {
      type: Sequelize.INTEGER,
      references: {
        model: "boards",
        key: "id",
      },
    },
  },
  {
    timestamps: true, // timestamps will now be true
  }
);

export default Log;
