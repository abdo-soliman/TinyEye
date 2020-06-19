'use strict';

module.exports = {
  up: async(queryInterface, Sequelize) => {
    /*
      Add altering commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.createTable('users', { id: Sequelize.INTEGER });
    */

  await queryInterface.createTable(
  'images',
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
  },
  {
    engine: 'InnoDB',                     // default: 'InnoDB'
    charset: 'utf8',                    // default: null
    schema: ''                      // default: public, PostgreSQL only.
  }
)
   },
  down: (queryInterface, Sequelize) => {
    /*
      Add reverting commands here.
      Return a promise to correctly handle asynchronicity.

      Example:
      return queryInterface.dropTable('users');
    */
    return queryInterface.dropTable('images');
  }
};
