import Models from "../../models/Model";
import ImageController from "./ImageController";
import HumanController from "./HumanController";
import fs from "fs";
import exec from "child_process";

class ModelController {
  addModel = async (path,boardId) => {
    await Models.create({
      mpath: path,
      boardId: boardId,
    });

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
    let data = fs.readFileSync(myDirectory + "/logs/log.json");
    let errors = JSON.parse(data).errors;
    if (errors.length != 0) {
      console.log("error exists while training model");
      errors.forEach((error) => {
        console.log(error.code);
      });
      return false;
    }
    else {
      return true;
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

  saveModel = async (myDirectory,boardId) => {
    if (this.getModelbyBoard(boardId)){
      await this.updateModel(myDirectory,boardId);
    }
    else{
      await this.addModel(myDirectory,boardId);
    }

  }
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
    if (this.trainModel(myDirectory)){
      this.saveModel(myDirectory);
      return res.json({ msg: "model created successfully" });
      // TODO Call function to send model from myDirectory+"/models/model.pt" to rassb.
    }
    else {
      return res.json({ msg: "Error exists during model creation" });
    }
    
  };


  updateModel = async (path,boardId) => {
    await Models.update(
      {
        mpath: path,
        boardId: boardId,
      },
      {
        where: {
          boardId: boardId,
        },
      }
    );
  };

  getModelbyBoard = async (boardId) => {
    var model = await Models.findAll({
      where: {
        boardId: boardId,
      },
    });
    return model[0].dataValues;
  };

  getModelbyBoard = async (boardId) => {
    var model = await Models.findAll({
      where: {
        boardId: boardId,
      },
    });
    if (Object.keys(model).length === 0){
      return false;

    }
    else
    {
      return true;
    }
    
    
  };
}

export default ModelController;
