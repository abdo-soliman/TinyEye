import validate from "../requests";

class HomeController {
  index = (req, res) => {
    validate(req, res);

    return res.json({ welcome: "Hello World!" });
  };

  about = (req, res) => {
    return res.send("About!");
  };
}

export default HomeController;
