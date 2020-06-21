import validate from "../requests";
import Humans from "../../models/Human";
import Sequelize from "sequelize";

class HumanController {
  async addHuman (name, classId,boardId) {
    await Humans.create({
      Name: name,
      classId: classId,
      boardId: boardId,
    });
    // make it using try to handle errors 
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


  getHumanbyboard = async (req, res) => {
    var humans = await Humans.findAll({
      where: {
        boardId: req.body.boardId,
      },
    });
    return res.json({humans});
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
