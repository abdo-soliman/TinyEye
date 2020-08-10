import Human from "../../models/Human";
import Models from "../../models/Model";
import ImageController from "./ImageController";
import fs from "fs";
import ServerLogger from "../modules/ServerLogger";
import ModelController from "./ModelController";
const config = require("../../config/config.json");
class HumanController {
  index = async (req, res) => {
    const boardId = req.user.boardId;
    const humans = await Human.findAll({
      where: {
        boardId: boardId,
      },
    });

    if (humans) {
      let data = [];
      for (let index = 0; index < humans.length; index++) {
        const human = humans[index];
        data.push({ ...human.dataValues, image: await human.get("image") });
      }
      return res.json(data);
    }
    return res.json([]);
  };

  show = async (req, res) => {
    let human = await this.getHumanbyid(req.body.id);
    if (human) {
      human = { ...human.dataValues, images: await human.get("images") };
      return res.json(human);
    }
    return res.status(404).json({});
  };

  addHuman = async (name, myDirectory, boardId) => {
    const human = await Human.create({
      name: name,
      dirPath: myDirectory,
      boardId: boardId,
    });
    // make it using try to handle errors
    return human;
  };

  makedirectory = async (name) => {
    await fs.mkdir(name, { recursive: true }, async function (err) {
      if (err) {
        await ServerLogger.error(err);
      } else {
        await ServerLogger.log("New directory successfully created " + name);
      }
    });
    return;
  };

  prepareData = async (req, res) => {
    const imagecontroller = new ImageController();
    const modelController = new ModelController();
    const name = req.body.name.replace(" ", "_");
    const now = Date.now();
    let myDirectory = `${__dirname}/../../storage/board_${req.user.boardId}/Images/${name}_${now}`;
    // // save in data base in humans indicate new class
    var human = await this.addHuman(
      req.body.name,
      myDirectory,
      req.user.boardId
    );
    await this.makedirectory(myDirectory);
    for (let i = 0; i < req.files.length; i++) {
      const imagePath = `${myDirectory}/image_${i}`;
      const imageUrl = `${config.url}/board_${req.user.boardId}/Images/${name}_${now}/image_${i}`;
      fs.rename(req.files[i].path, imagePath, async (err) => {
        if (err) {
          await ServerLogger.error(err);
          await this.deleteHumanbyid(human.dataValues.id);
          fs.rmdir(myDirectory, { recursive: true }, () => {});
          return res.status(500).json("Failed to add the human");
        }
      });
      // save in database in images indicate new image
      await imagecontroller.addImage(
        imagePath,
        imageUrl,
        req.user.boardId,
        human.dataValues.id
      );
    }
    await this.deleteModel(human.dataValues.boardId);
    if (req.body.train) {
      const trained = await modelController.createModel(req);
      if (!trained) {
        return res.json({
          msg: "Model is already training we will notify you when we done",
        });
      }
      return res.json({
        msg:
          "data is prepared successfully and model is training we will notify you when we done",
      });
    }
    return res.json({ msg: "data is prepared successfully and human added" });
  };

  removeHuman = async (req, res) => {
    const modelController = new ModelController();
    //assumed that request contains user and human objects
    const human = await this.getHumanbyid(req.body.humanId);
    if (human) {
      await this.deleteHumanbyid(req.body.humanId);
      await this.deleteModel(human.dataValues.boardId);
      const myDirectory = human.dataValues.dirPath;
      if (fs.existsSync(myDirectory)) {
        fs.rmdir(myDirectory, { recursive: true }, () => {});
        // await modelController.createModel(req);
        return res.json({
          msg:
            // "Person is deleted successfully and model is training we will notify you when we done",
            "Person is deleted successfully, Please press train model to update your data",
        });
      }
    }
    return res.status(404).json({
      msg: "Error this human is doesn't exists ",
    });
  };

  deleteHumanbyid = async (id) => {
    await Human.destroy({
      where: {
        id: id,
      },
    });
  };

  getHumanbyboard = async (boardId) => {
    var humans = await Human.findAll({
      where: {
        boardId: boardId,
      },
    });
    return humans;
  };

  deleteHumanbyname = async (req, res) => {
    var human = await Human.destroy({
      where: {
        name: req.body.name,
      },
    });
    return res.json({ human });
  };

  getHumanbyid = async (id) => {
    const human = await Human.findOne({
      where: {
        id: id,
      },
    });
    return human;
  };

  deleteModel = async (boardId) => {
    await Models.destroy({ where: { boardId: boardId } });
  };
}
export default HumanController;
