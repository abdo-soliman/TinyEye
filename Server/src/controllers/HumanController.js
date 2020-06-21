import validate from "../requests";
import Humans from "../../models/Human";
import Sequelize from "sequelize";

class HumanController {
  async addHuman (name, classId,boardId) {
    var human = await Humans.create({
      Name: name,
      classId: classId,
      boardId: boardId,
    });
    // make it using try to handle errors 
    return human.dataValues.id;
  };

  

  async getHumanCounts (boardId) {
      var count = await Humans.findAll({
      attributes: [[Sequelize.fn('COUNT', Sequelize.col('id')), 'hCount']],
      where: {
        boardId : boardId,
      },
    });
    return count[0].dataValues.hCount;
  };

  deleteHumanbyid = (req, res) => {
    Humans.destroy({
      where: {
        id: req.body.id,
      },
    });
    return res.json({ msg: "human deleted" });
  };


  async getHumanbyboard (boardId) {
    var humans = await Humans.findAll({
      where: {
        boardId: boardId,
      },
    });
    return humans;
  };

  deleteHumanbyname = async (req, res) => {
    var human = await Humans.destroy({
      where: {
        name: req.body.name,
      },
    });
    return res.json({ human });
  };

  getHuman = async (req, res) => {
    var human = await Humans.findAll({
      where: {
        id: req.body.id,
      },
    });
    return res.json({ human });
  };
}
export default HumanController;
