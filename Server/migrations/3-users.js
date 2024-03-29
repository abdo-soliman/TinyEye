"use strict";

module.exports = {
  up: async (queryInterface, Sequelize) => {
    await queryInterface.createTable(
      "users",
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
        name: Sequelize.STRING,
        expoToken: Sequelize.STRING,
        email: {
          type: Sequelize.STRING,
          indicesType: "UNIQUE",
          allowNull: false,
          unique: true,
          validate: {
            isEmail: {
              msg: "Must be a valid email address",
            },
          },
        },
        password: {
          type: Sequelize.STRING,
          allowNull: false,
        },
        //foreign key usage
        boardId: {
          type: Sequelize.INTEGER,
          references: {
            model: "boards",
            key: "id",
          },
          onUpdate: "cascade",
          onDelete: "cascade",
        },
      },
      {
        engine: "InnoDB", // default: 'InnoDB'
        charset: "utf8", // default: null
        schema: "", // default: public, PostgreSQL only.
      }
    );
  },

  down: (queryInterface, Sequelize) => {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.dropTable('users');
    */
    return queryInterface.dropTable("users");
  },
};
