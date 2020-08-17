import Image from "../../models/Image";

class ImageController {
  addImage = async (path, url, boardId, humanId) => {
    await Image.create({
      iPath: path,
      iUrl: url,
      boardId: boardId,
      humanId: humanId,
    });
    // make it using try to check for errors
  };

  deleteImage = (req, res) => {
    Image.destroy({
      where: {
        id: req.body.id,
      },
    });
    return res.json({ msg: "image deleted" });
  };

  deleteHumanImages = async (humanId) => {
    await Image.destroy({
      where: {
        humanId: humanId,
      },
    });
  };

  updateImage = (req, res) => {
    Image.update(
      {
        ipath: req.body.path,
        boardId: req.body.boardid,
      },
      {
        where: {
          id: req.body.id,
        },
      }
    );
    return res.json({ msg: "image updated" });
  };

  getImagebyboard = async (req, res) => {
    var images = await Image.findAll({
      where: {
        boardId: req.body.boardId,
      },
    });
    return res.json({ images });
  };

  getImagebyboardAndHuman = async (boardId, humanId) => {
    var images = await Image.findAll({
      where: {
        boardId: boardId,
        humanId: humanId,
      },
    });
    return images;
  };

  getImagebyhuman = async (req, res) => {
    var images = await Image.findAll({
      where: {
        humanId: req.body.humanId,
      },
    });
    return res.json({ images });
  };
}

export default ImageController;
