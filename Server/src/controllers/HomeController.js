import Board from "../../models/Board";

class HomeController {
  index = (req, res) => {
    // validate(req, res);

    return res.json({ welcome: "Hello World!" });
  };

  about = (req, res) => {
    Board.findAll().then((boards) => {
      // console.log(boards);
      return res.json(boards);
    });
  };
}

export default HomeController;
