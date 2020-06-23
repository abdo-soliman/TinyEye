import Human from "../../models/Human";
import ImageController from "./ImageController";
import fs from "fs";
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

  addHuman = async (name, myDirectory, boardId) => {
    const human = await Human.create({
      name: name,
      dirPath: myDirectory,
      boardId: boardId,
    });
    // make it using try to handle errors
    return human;
  };

  makedirectory = (name) => {
    fs.mkdir(name, { recursive: true }, function (err) {
      if (err) {
        console.log(err);
      } else {
        console.log("New directory successfully created.");
      }
    });
  };

  prepareData = async (req, res) => {
    const imagecontroller = new ImageController();
    const name = req.body.name.replace(" ", "_");
    const now = Date.now();
    let myDirectory = `${__dirname}/../../storage/board_${req.user.boardId}/Images/${name}_${now}`;
    // // WHY??????????????????????????????
    myDirectory = myDirectory.replace(" ", "''");
    // // save in data base in humans indicate new class
    var human = await this.addHuman(
      req.body.name,
      myDirectory,
      req.user.boardId
    );
    this.makedirectory(myDirectory);
    for (let i = 0; i < req.body.images.length; i++) {
      const imagePath = `${myDirectory}/image_${i}`;
      const imageUrl = `${config.url}/board_${req.user.boardId}/Images/${name}_${now}/image_${i}`;
      fs.writeFile(imagePath, req.body.images[i], "base64", async (err) => {
        console.log(err);
        if (err) {
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
    return res.json({ msg: "data is prepared successfully and human added " });
  };

  removeHuman = async (req, res) => {
    //assumed that request contains user and human objects
    const human = await this.getHumanbyid(req.body.humanId);
    console.log("human", human);
    if (human) {
      await this.deleteHumanbyid(req.body.humanId);
      const myDirectory = human.dataValues.dirPath;
      if (fs.existsSync(myDirectory)) {
        fs.rmdir(myDirectory, { recursive: true }, () => {});
        return res.json({
          msg: "human is deleted and its images from both database and storage",
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
}
export default HumanController;
