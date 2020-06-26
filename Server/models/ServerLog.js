import db from "./db";
import Sequelize from "sequelize";

const ServerLog = db.define(
  "server-logs",
  {
    type: Sequelize.STRING,
    data: Sequelize.TEXT,
  },
  {
    timestamps: true, // timestamps will now be true
  }
);

export default ServerLog;
