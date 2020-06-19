import db from "./db";
import Sequelize from "sequelize";

const Board = db.define(
  "board",
  {
    UUID: {
      type: Sequelize.UUID,
    },
  },
  {
    timestamps: true, // timestamps will now be true
  }
);

export default Board;
