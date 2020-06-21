import db from "./db";
import Sequelize from "sequelize";

const Users = db.define(
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

export default Users;
