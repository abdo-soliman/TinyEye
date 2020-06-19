import Sequelize from "sequelize";
const env = process.env.NODE_ENV || "development";
const config = require("../config/config.json")[env];

const db = new Sequelize(config.database, config.username, config.password, {
  host: config.host,
  dialect: config.dialect,

  pool: {
    max: 5,
    min: 0,
    idle: 10000,
  },
});

export default db;
