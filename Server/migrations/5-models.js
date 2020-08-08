"use strict";

module.exports = {
  up: async (queryInterface, Sequelize) => {
    /*
      Add altering commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.createTable('users', { id: Sequelize.INTEGER });
    */
    await queryInterface.createTable(
      "models",
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
        mPath: Sequelize.STRING,
        mUrl: Sequelize.STRING,
        mapPath: Sequelize.STRING,
        mapUrl: Sequelize.STRING,
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
    return queryInterface.dropTable("models");
  },
};
