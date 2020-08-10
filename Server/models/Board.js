import db from "./db";
import Sequelize from "sequelize";

const Board = db.define(
  "board",
  {
    UUID: {
      type: Sequelize.UUID,
    },
    training: {
      type: Sequelize.BOOLEAN,
      defaultValue: false,
    },
  },
  {
    timestamps: true, // timestamps will now be true
  }
);

export default Board;
