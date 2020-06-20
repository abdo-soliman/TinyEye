
import validate from "../requests";
import Images from "../../models/Image";

class ImageController {
  addModel = (req, res) => {
  	Models.create({
        ipath: req.body.path,
        boardId:req.body.boardId,
        humanId:req.body.humanId
    })

  return res.json({ msg :"image inserted"});
  };

  deleteImage = (req, res) => {
  	Images.destroy({
	  where: {
	    id: req.body.id
	  }
})
  return res.json({ msg :"image deleted" });
  };

  updateImage = (req, res) => {
  	Images.update({
	  ipath: req.body.path,
    boardId:req.body.boardid
	}, {
	  where: {
	    id: req.body.id
	  }
})
  return res.json({ msg :"image updated" });
  };

  getImagebyboard = async (req, res) =>{
  var images = await Images.findAll({
    where: {
      boardId: req.body.boardId
    }
})
  return res.json({ images });
  };

  

  getImagebyhuman = async (req, res) =>{
	var images = await Images.findAll({
	  where: {
	    humanId: req.body.humanId
	  }
})
  return res.json({ images });
  };
}

export default ImageController;


