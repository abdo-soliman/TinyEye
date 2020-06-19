import validate from "../requests";

class AuthController {
  register = (req, res) => {
    validate(req, res)
    console.log(req)
    return res.json({"Hi": "Hello"});
  };

  login = (req, res) => {
    return res.json({"Hi": "Hello"});
  };
}

export default AuthController;
