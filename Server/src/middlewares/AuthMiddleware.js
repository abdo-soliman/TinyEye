import jwt from "jsonwebtoken";

const JWT_SECRET = require("../../config/config.json")["JWT_SECRET"];

const AuthMiddleware = (req, res, next) => {
  const authHeader = req.headers["authorization"];
  const token = authHeader && authHeader.split(" ")[1];
  if (token == null) {
    return res
      .status(401)
      .json({ errors: [{ msg: "Unauthorized", param: "result" }] });
  }
  jwt.verify(token, JWT_SECRET, (err, user) => {
    if (err) {
      return res
        .status(403)
        .json({ errors: [{ msg: "Unauthorized", param: "result" }] });
    }
    req.user = user;
    next();
  });
};

export default AuthMiddleware;
