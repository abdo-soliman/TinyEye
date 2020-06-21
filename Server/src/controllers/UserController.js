import validate from "../requests";
import Users from "../../models/User";

class UserController {
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
