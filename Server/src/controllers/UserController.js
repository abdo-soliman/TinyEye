import validate from "../requests";
import Users from "../../models/User";
import HumanController from "./HumanController";
import ImageController from "./ImageController";
import fs from "fs";
import List from "collections/list";
var humancontroller = new HumanController();
var imagecontroller = new ImageController();
class UserController {
  addUser = (req, res) => {
    Users.create({
      Name: req.body.name,
      email: req.body.email,
      password: req.body.password,
      boardId: req.body.boardId,
    });

    return res.json({ msg: "user inserted" });
  };

  deleteUser = (req, res) => {
    Users.destroy({
      where: {
        id: req.body.id,
      },
    });
    return res.json({ msg: "user deleted" });
  };

  updateEmail = (req, res) => {
    Users.update(
      {
        email: req.body.email,
      },
      {
        where: {
          id: req.body.id,
        },
      }
    );
    return res.json({ msg: "user email updated" });
  };

  async mappingToFile(Images, myDirectory, classId) {
    var i;
    for (i = 0; i < Images.length; i++) {
      await fs.appendFileSync(
        myDirectory + "/trainFile",
        Images[i].dataValues.iPath + " , " + classId + "\n",
        function (err) {
          console.log(err);
        }
      );
    }
  }
  createModel = async (req, res) => {
    var humans = await humancontroller.getHumanbyboard(req.user.boardId);
    var i;
    var myDirectory = "./storage/board_" + req.user.boardId;
    for (i = 0; i < humans.length; i++) {
      console.log(humans[i].id);
      console.log("here");
      var Images = await imagecontroller.getImagebyboardAndHuman(
        humans[i].dataValues.boardId,
        humans[i].dataValues.id
      );
      console.log(Images[i].dataValues);
      this.mappingToFile(Images, myDirectory, humans[i].classId);
      // humansArray[i]= humans[i].dataValues;
    }
  };

  makedirectory(name) {
    fs.mkdir(name, { recursive: true }, function (err) {
      if (err) {
        console.log(err);
      } else {
        console.log("New directory successfully created.");
      }
    });
  }

  checkSpaces(name) {
    if (name.indexOf(" ") >= 0) {
      name = name.replace(" ", "_");
    }
    return name;
  }

  prepareData = async (req, res) => {
    var classId = await humancontroller.getHumanCounts(req.user.boardId);
    const name = this.checkSpaces(req.body.name);
    var myDirectory =
      "./storage/board_" + req.user.boardId + "/Images/" + name + "_" + classId;
    this.makedirectory(myDirectory);
    // save in data base in humans indicate new class
    var humanId = await humancontroller.addHuman(
      req.body.name,
      classId,
      req.user.boardId
    );
    var i;
    for (i = 0; i < req.body.images.length; i++) {
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
  };

  updatePassword = (req, res) => {
    Users.update(
      {
        password: req.body.password,
      },
      {
        where: {
          id: req.body.id,
        },
      }
    );
    return res.json({ msg: "user password updated" });
  };

  getUser = async (req, res) => {
    var user = await Users.findAll({
      where: {
        id: req.body.id,
      },
    });
    return res.json({ user });
  };
}

export default UserController;
