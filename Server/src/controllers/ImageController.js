
import validate from "../requests";
import Images from "../../models/Image";

class ImageController {
  async addImage (path, boardId,humanId) {
  	await Images.create({
        iPath: path,
        boardId:boardId,
        humanId:humanId
    })
    // make it using try to check for errors 
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


