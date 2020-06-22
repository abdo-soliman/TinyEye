import validate from "../requests";
import Models from "../../models/Model";
import ImageController from "./ImageController";
import HumanController from "./HumanController";
import fs from "fs";
class ModelController {
  addModel = (req, res) => {
    Models.create({
      mpath: req.body.path,
      boardId: req.body.boardid,
    });

    return res.json({ msg: "model inserted" });
  };

  deleteModel = (req, res) => {
    Models.destroy({
      where: {
        id: req.body.id,
      },
    });

    return res.json({ msg: "model deleted" });
  };

  mappingToFile = async (images, myDirectory, classId, deletionFile) => {
    try {
      if (deletionFile && fs.existsSync(myDirectory + "/trainFile")) {
        fs.unlinkSync(myDirectory + "/trainFile");
        fs.unlinkSync(myDirectory + "/testFile");
      }
    } catch (err) {
      console.error(err);
    }

    if (deletionFile && fs.existsSync(myDirectory + "/testFile")) {
      fs.unlinkSync(myDirectory + "/testFile");
    }

    var trainLength = Math.ceil(0.8 * images.length);

    for (let i = 0; i < trainLength; i++) {
      await fs.appendFileSync(
        myDirectory + "/trainFile",
        images[i].dataValues.iPath + " " + classId + "\n",
        function (err) {
          console.log(err);
        }
      );
    }

    for (let i = trainLength; i < images.length; i++) {
      await fs.appendFileSync(
        myDirectory + "/testFile",
        images[i].dataValues.iPath + " " + classId + "\n",
        function (err) {
          console.log(err);
        }
      );
    }
  };

  createModel = async (req, res) => {
    const humancontroller = new HumanController();
    const imagecontroller = new ImageController();
    var humans = await humancontroller.getHumanbyboard(req.user.boardId);

    var myDirectory = __dirname + "/../../storage/board_" + req.user.boardId;
    var deletionFile = true; // just will be true for only the first human 
    humans.forEach(async (human) => {
      var images = await imagecontroller.getImagebyboardAndHuman(
        human.dataValues.boardId,
        human.dataValues.id
      );
      this.mappingToFile(images, myDirectory, human.classId,deletionFile);
      deletionFile = false;
    });
    return res.json({ msg: "model created successfully" });
  };

  updateModel = (req, res) => {
    Models.update(
      {
        mpath: req.body.path,
        boardId: req.body.boardid,
      },
      {
        where: {
          id: req.body.id,
        },
      }
    );
    return res.json({ msg: "model updated" });
  };

  getModel = async (req, res) => {
    var model = await Models.findAll({
      where: {
        id: req.body.id,
      },
    });
    return res.json({ model });
  };
}

export default ModelController;
