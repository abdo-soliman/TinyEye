import jwt from "jsonwebtoken";
import User from "../../models/User";

const JWT_SECRET = require("../../config/config.json")["JWT_SECRET"];

const AuthMiddleware = (req, res, next) => {
  const authHeader = req.headers["authorization"];
  const token = authHeader && authHeader.split(" ")[1];
  if (token == null) {
    return res
      .status(401)
      .json({ errors: [{ msg: "Unauthorized", param: "result" }] });
  }
  jwt.verify(token, JWT_SECRET, async (err, user) => {
    let databaseUser = null;
    if (user) {
      databaseUser = await User.findOne({
        where: { id: user.id, email: user.email, name: user.name },
      });
    }
    if (err || !databaseUser) {
      return res
        .status(403)
        .json({ errors: [{ msg: "Unauthorized", param: "result" }] });
    }
    req.user = databaseUser.dataValues;
    next();
  });
};

export default AuthMiddleware;
