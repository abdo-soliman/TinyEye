import db from "./db";
import Sequelize from "sequelize";
import Human from "./Human";

const Image = db.define(
  "images",
  {
    id: {
      type: Sequelize.INTEGER,
      primaryKey: true,
      autoIncrement: true
    },
    createdAt: {
      type: Sequelize.DATE
    },
    updatedAt: {
      type: Sequelize.DATE
    },
    iPath: Sequelize.STRING,
    iUrl: Sequelize.STRING,
    boardId: {
        type: Sequelize.INTEGER,
        references: {
            model: 'boards',
            key: 'id'
        },
        onUpdate: 'cascade',
        onDelete: 'cascade'
    },
    humanId: {
        type: Sequelize.INTEGER,
        references: {
            model: 'humans',
            key: 'id'
        },
        onUpdate: 'cascade',
        onDelete: 'cascade'
    }
  }
);

// Image.belongsTo(Human)

export default Image;