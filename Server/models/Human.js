import db from "./db";
import Sequelize from "sequelize";
import Image from "./Image";

const Human = db.define(
  "humans",
  {
    id: {
      type: Sequelize.INTEGER,
      primaryKey: true,
      autoIncrement: true,
    },
    name: Sequelize.STRING,
    dirPath: Sequelize.STRING,
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
    getterMethods: {
      image: async function () {
        const image = await Image.findOne({
          where: {
            humanId: this.getDataValue("id"),
          },
        });
        return image.dataValues.iUrl;
      },
      images: async function () {
        const images = await Image.findAll({
          where: {
            humanId: this.getDataValue("id"),
          },
        });
        return images;
      },
    },
  }
);

// Human.belongsTo(Image, { as: "images", foreignKey: "humanId" });

export default Human;
