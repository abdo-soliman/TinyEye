import Models from "../../models/Model";
import ImageController from "./ImageController";
import HumanController from "./HumanController";
import fs from "fs";
import exec from "child_process";
import pushNotification from "../notifications";
import User from "../../models/User";
import Board from "../../models/Board";
import { boardNsp } from "../sockets";
import ServerLogger from "../modules/ServerLogger";
const config = require("../../config/config.json");

class ModelController {
  addModel = async (path, url, mapFile, mapUrl, boardId) => {
    return await Models.create({
      mPath: path,
      mUrl: url,
      mapPath: mapFile,
      mapUrl: mapUrl,
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
        ServerLogger.error(err.message);
      } else {
        ServerLogger.log("New directory successfully created " + name);
      }
    });
  };

  getBoardUsers = async (boardId) => {
    const users = await User.findAll({ where: { boardId: boardId } });
    return users;
  };

  sendSucessNotifications = async (boardId) => {
    const users = await this.getBoardUsers(boardId);
    const tokens = [];
    for (let index = 0; index < users.length; index++) {
      const user = users[index];
      tokens.push(user.dataValues.expoToken);
    }
    if (tokens !== []) {
      await pushNotification(
        tokens,
        "Model Prepared",
        "Model prepared and downloaded to the board"
      );
    }
  };

  sendFailNotifications = async (boardId) => {
    const users = await this.getBoardUsers(boardId);
    const tokens = [];
    for (let index = 0; index < users.length; index++) {
      const user = users[index];
      tokens.push(user.dataValues.expoToken);
    }
    if (tokens !== []) {
      await pushNotification(
        tokens,
        "Model Failed",
        "Model failed please try again"
      );
    }
  };

  getBoard = async (boardId) => {
    const board = await Board.findOne({ where: { id: boardId } });
    return board;
  };

  sendCompleteSignalToBoard = async (boardId, modelUrl, mapUrl) => {
    console.log("start Send to board");
    boardNsp.emit("model", {
      model: modelUrl,
      map: mapUrl,
      updated: true,
    });
    // });
  };

  trainingModel = async (req, myDirectory, delimiter, boardId) => {
    if (!fs.existsSync(myDirectory + "/models")) {
      this.makedirectory(myDirectory + "/models");
    }

    if (!fs.existsSync(myDirectory + "/logs")) {
      this.makedirectory(myDirectory + "/logs");
    }
    try {
      const trainFile = `${myDirectory}/trainFile`;
      const testFile = `${myDirectory}/testFile`;
      const mapFile = `${myDirectory}/mapFile`;
      const mapUrl = `${config.url}/board_${boardId}/mapFile`;
      const modelFile = `${myDirectory}/models/model.pt`;
      const modelUrl = `${config.url}/board_${boardId}/models/model.pt`;
      const logFile = `${myDirectory}/logs/log.json`;
      const executeStatement = `~/tinyeye-server/bin/tinyeye-server_module 2> /dev/null --server-log-path ~/tinyeye-server.log --mtcnn-models-dir ~/tinyeye-server/models/mtcnn --mfn-model-path ~/tinyeye-server/models/mobilefacenet.pt --train-map-path ${trainFile} --test-map-path ${testFile} --output-model-path ${modelFile} --json-log-path ${logFile} --mapping-file-delimiter ${delimiter} --log true`;
      exec.exec(executeStatement, async (error, stdout, stderr) => {
        if (error !== null) {
          ServerLogger.error(error.message);
          await this.sendFailNotifications(boardId);
        } else {
          if (stderr) {
            await this.sendFailNotifications(boardId);
            ServerLogger.stdError(stderr);
          } else {
            ServerLogger.modelLog(stdout);
            const model = await this.addModel(
              modelFile,
              modelUrl,
              mapFile,
              mapUrl,
              boardId
            );
            await this.sendCompleteSignalToBoard(
              boardId,
              model.dataValues.mUrl,
              mapUrl
            );
            await this.sendSucessNotifications(boardId);
          }
        }
        this.updateBoardTraining(false, req.user.boardId);
      });
    } catch (error) {
      ServerLogger.error(error.message);
      this.updateBoardTraining(false, req.user.boardId);
    }
    return true;
  };

  mappingToFile = async (images, myDirectory, classId, delimiter) => {
    var trainLength = Math.ceil(0.8 * images.length);
    for (let i = 0; i < trainLength; i++) {
      fs.appendFileSync(
        myDirectory + "/trainFile",
        images[i].dataValues.iPath + delimiter + classId + "\n",
        async function (err) {
          ServerLogger.error(err.message);
        }
      );
    }

    for (let i = trainLength; i < images.length; i++) {
      await fs.appendFileSync(
        myDirectory + "/testFile",
        images[i].dataValues.iPath + delimiter + classId + "\n",
        function (err) {
          ServerLogger.error(err.message);
        }
      );
    }
  };

  deleteFile = (fileName) => {
    fs.existsSync(fileName) && fs.unlinkSync(fileName);
  };

  createModel = async (req) => {
    const board = await this.getBoard(req.user.boardId);
    if (board.training) {
      return false;
    }
    this.updateBoardTraining(true, req.user.boardId);
    const humancontroller = new HumanController();
    const imagecontroller = new ImageController();
    var humans = await humancontroller.getHumanbyboard(req.user.boardId);
    await humancontroller.deleteModel(req.user.boardId);
    var myDirectory = `${__dirname}/../../storage/board_${req.user.boardId}`;
    const delimiter = ",";
    this.deleteFile(`${myDirectory}/trainFile`);
    this.deleteFile(`${myDirectory}/testFile`);
    this.deleteFile(`${myDirectory}/mapFile`);
    humans.forEach(async (human, index) => {
      var images = await imagecontroller.getImagebyboardAndHuman(
        human.dataValues.boardId,
        human.dataValues.id
      );
      await fs.appendFileSync(
        myDirectory + "/mapFile",
        human.dataValues.name + "=" + index + "\n",
        async function (err) {
          ServerLogger.error(err.message);
        }
      );
      this.mappingToFile(images, myDirectory, index, delimiter);
    });
    await this.trainingModel(req, myDirectory, delimiter, req.user.boardId);
  };

  trainModel = async (req, res) => {
    const trained = await this.createModel(req);
    if (!trained) {
      return res.json({
        msg: "Model is already training we will notify you when we done",
      });
    }
    return res.json({
      msg: "Model is training we will notify you when we done",
    });
  };

  updateBoardTraining = async (training, boardId) => {
    await Board.update({ training: training }, { where: { id: boardId } });
  };

  // updateModel = (req, res) => {
  //   Models.update(
  //     {
  //       mpath: req.body.path,
  //       boardId: req.body.boardid,
  //     },
  //     {
  //       where: {
  //         id: req.body.id,
  //       },
  //     }
  //   );
  //   return res.json({ msg: "model updated" });
  // };

  getModel = async (req, res) => {
    var model = await Models.findAll({
      where: {
        id: req.body.id,
      },
    });
    return res.json({ model });
  };

  show = async (req, res) => {
    var model = await Models.findOne({
      where: {
        boardId: req.user.boardId,
      },
    });
    return res.json(model);
  };
}

export default ModelController;
