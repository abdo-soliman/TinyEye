import Models from "../../models/Model";
import ImageController from "./ImageController";
import HumanController from "./HumanController";
import fs from "fs";
import exec from "child_process";

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

  makedirectory = (name) => {
    fs.mkdir(name, { recursive: true }, function (err) {
      if (err) {
        console.log(err);
      } else {
        console.log("New directory successfully created.");
      }
    });
  };

  trainModel = async (myDirectory) => {
    if (!fs.existsSync(myDirectory + "/models")) {
      this.makedirectory(myDirectory + "/models");
    }

    if (!fs.existsSync(myDirectory + "/logs")) {
      this.makedirectory(myDirectory + "/logs");
    }
    try {
      const trainFile = `${myDirectory}/trainFile`;
      const testFile = `${myDirectory}/testFile`;
      const modelFile = `${myDirectory}/models/model.pt`;
      const logFile = `${myDirectory}/logs/log.json`;
      const executeStatement = `tinyModelBuilder --train-map-path ${trainFile} --test-map-path ${testFile} --output-model-path ${modelFile} --json-log-path ${logFile}`;
      exec.exec(executeStatement, (error, stdout, stderr) => {
        console.log("stdout: " + stdout);
        console.log("stderr: " + stderr);
        if (error !== null) {
          console.log("exec error: " + error);
        }
      });
    } catch (error) {
      console.log("exec error: " + error);
    }
  };

  mappingToFile = async (images, myDirectory, classId) => {
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

  deleteFile = (fileName) => {
    fs.existsSync(fileName) && fs.unlinkSync(fileName);
  };

  createModel = async (req, res) => {
    const humancontroller = new HumanController();
    const imagecontroller = new ImageController();
    var humans = await humancontroller.getHumanbyboard(req.user.boardId);
    var myDirectory = `${__dirname}/../../storage/board_${req.user.boardId}`;
    myDirectory = myDirectory.replace(" ", "''");
    this.deleteFile(`${myDirectory}/trainFile`);
    this.deleteFile(`${myDirectory}/testFile`);
    humans.forEach(async (human, index) => {
      var images = await imagecontroller.getImagebyboardAndHuman(
        human.dataValues.boardId,
        human.dataValues.id
      );
      this.mappingToFile(images, myDirectory, index);
    });
    this.trainModel(myDirectory);
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
