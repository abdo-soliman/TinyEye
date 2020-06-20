import validate from "../requests";
import Users from "../../models/User";
var fs = require("fs");

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

  createModel = (req, res) => {
	
  };

  
  prepareData = (req, res) => {
	
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
