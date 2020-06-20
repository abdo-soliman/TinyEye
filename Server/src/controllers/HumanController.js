import validate from "../requests";
import Humans from "../../models/Human";

class HumanController {
  addHuman = (req, res) => {
    Humans.create({
      Name: req.body.name,
      classId: req.body.classId,
      boardId: req.body.boardId,
    });

    return res.json({ msg: "human inserted" });
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
