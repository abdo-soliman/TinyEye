import validate from "../requests";
import Humans from "../../models/Human";
import Sequelize from "sequelize";
import ImageController from "./ImageController";
import fs from "fs";
class HumanController {
  async addHuman(name, classId, boardId) {
    var human = await Humans.create({
      name: name,
      classId: classId,
      boardId: boardId,
    });
    // make it using try to handle errors
    return human.dataValues.id;
  }

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
    var classId = await this.getHumanCounts(req.user.boardId);
    const name = req.body.name.replace(" ", "_");
    //const name = req.body.name;
    const myDirectory =
      __dirname +
      "/../../storage/board_" +
      req.user.boardId +
      "/Images/" +
      name +
      "_" +
      classId;
    this.makedirectory(myDirectory);
    // save in data base in humans indicate new class
    var humanId = await this.addHuman(req.body.name, classId, req.user.boardId);
    for (let i = 0; i < req.body.images.length; i++) {
      fs.writeFile(
        myDirectory + "/image_" + i,
        req.body.images[i],
        "base64",
        function (err) {
          console.log(err);
        }
      );
      // save in database in images indicate new image
      await imagecontroller.addImage(
        myDirectory + "/image_" + i,
        req.user.boardId,
        humanId
      );
    }
    return res.json({ msg: "data is prepared successfully and human added " });
  };


  removeHuman = async (req, res) => {
    //assumed that request contains user and human objects
    const imagecontroller = new ImageController();
    await imagecontroller.deleteHumanImages(req.body.human.humanId);
    this.deleteHumanbyid(req.body.human.humanId);
    const myDirectory =
      __dirname +
      "/../../storage/board_" +
      req.user.boardId +
      "/Images/" +
      req.body.human.name +
      "_" +
      req.body.human.classId;
      console.log(myDirectory);

    
    const files = fs.readdirSync(myDirectory);
    files.forEach(filename => {
      fs.unlinkSync(myDirectory+"/"+filename) 
    });
    // remove its images from databse and storage
    
    fs.rmdir(myDirectory, function(err) {
      if (err) {
        throw err
      } else {
        console.log("Successfully removed the empty directory!")
      }
    })

    return res.json({
      msg: "human is deleted and its images from both database and storage",
    });
  };

  getHumanCounts = async (boardId) => {
    var count = await Humans.findAll({
      attributes: [[Sequelize.fn("COUNT", Sequelize.col("id")), "hCount"]],
      where: {
        boardId: boardId,
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

  async getHumanbyboard(boardId) {
    var humans = await Humans.findAll({
      where: {
        boardId: boardId,
      },
    });
    return humans;
  }

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
