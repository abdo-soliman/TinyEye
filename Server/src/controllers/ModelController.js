
import validate from "../requests";
import Models from "../../models/Model";

class ModelController {
  addModel = (req, res) => {
  	Models.create({
        mpath: req.body.path,
        boardId:req.body.boardid
    })

  return res.json({ msg :"model inserted"});
  };

  deleteModel = (req, res) => {
  	Models.destroy({
	  where: {
	    id: req.body.id
	  }
})
  return res.json({ msg :"model deleted" });
  };

  updateModel = (req, res) => {
  	Models.update({
	  mpath: req.body.path,
    boardId:req.body.boardid
	}, {
	  where: {
	    id: req.body.id
	  }
})
  return res.json({ msg :"model updated" });
  };

  

  getModel = async (req, res) =>{
	var model = await Models.findAll({
	  where: {
	    id: req.body.id
	  }
})
  return res.json({ model });
  };
}

export default ModelController;


