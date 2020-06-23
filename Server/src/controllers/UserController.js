import User from "../../models/User";

class UserController {
  deleteUser = async (req, res) => {
    await User.destroy({
      where: {
        id: req.body.id,
      },
    });
    return res.json({ msg: "user deleted" });
  };

  updateEmail = async (req, res) => {
    const user = await this.getUserByID(req.body.id);
    if (user) {
      const user = await User.update(
        {
          email: req.body.email,
        },
        {
          where: {
            id: req.body.id,
          },
        }
      );
      return res.json({ msg: "user email updated", user: user.dataValues });
    }
    return res.status(404).json({ msg: "User not found!" });
  };

  updatePassword = async (req, res) => {
    if (req.body.password === req.body.passwordConfirmation) {
      const user = await this.getUserByID(req.body.id);
      if (user) {
        await User.update(
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
      }
      return res.status(404).json({ msg: "User not found!" });
    }
    return res
      .status(402)
      .json({ msg: "Wrong password confirmation", param: "password" });
  };

  getUserByID = async (id) => {
    const user = await User.findOne({
      where: {
        id: id,
      },
    });
    return user;
  };

  getUser = async (req, res) => {
    const user = await this.getUserByID(req.body.id);
    if (user) {
      return res.json(user.dataValues);
    }
    return res.status(404).json({ msg: "User not found!" });
  };
}

export default UserController;
