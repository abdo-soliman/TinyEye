import db from "./db";
import Sequelize from "sequelize";

const User = db.define(
  "user",
  {
    name: Sequelize.STRING,
    email: {
      type: Sequelize.STRING,
    },
    password: {
      type: Sequelize.STRING,
    },
    boardId: {
      type: Sequelize.INTEGER,
    },
  },
  {
    timestamps: true, // timestamps will now be true
  }
);

export default User;
