import Board from "../../models/Board";
import Models from "../../models/Model";

class ModelMan {
  static fetchUpdatedModel = async (UUID) => {
    const board = await Board.findOne({ where: { UUID: UUID } });
    if (board) {
      const id = board.dataValues.id;
      const model = await Models.findOne({
        where: { boardId: id, updated: true },
      });
      return model;
    }
    return null;
  };
}

export default ModelMan;
