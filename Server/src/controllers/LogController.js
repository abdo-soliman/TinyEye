import Log from "../../models/Log";

class LogController {
  index = async (req, res) => {
    const boardId = req.user.boardId;
    const log = await Log.findAll({
      where: { boardId: boardId },
      order: [["id", "DESC"]],
    });
    return res.json(log);
  };

  show = async (req, res) => {
    const id = req.body.id;
    const log = await Log.findOne({ where: { id: id } });
    return res.json(log);
  };

  markAsRead = async (req, res) => {
    const id = req.body.id;
    await Log.update({ readAt: Date.now() }, { where: { id: id } });
    const log = await Log.findOne({ where: { id: id } });
    return res.json(log);
  };

  delete = async (req, res) => {
    const id = req.body.id;
    const log = await Log.destroy({ where: { id: id } });
    return res.json(log);
  };
}

export default LogController;
