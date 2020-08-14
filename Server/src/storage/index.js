import multer from "multer";
import fs from "fs";

const Storage = multer.diskStorage({
  destination(req, file, callback) {
    fs.mkdir(
      `${__dirname}/../../storage/board_${req.user.boardId}/Images`,
      { recursive: true },
      function (err) {
        if (err) {
          console.log(err);
        }
      }
    );
    callback(
      null,
      `${__dirname}/../../storage/board_${req.user.boardId}/Images`
    );
  },
  filename(req, file, callback) {
    callback(null, `${Date.now()}_${file.originalname}`);
  },
});

const upload = multer({ storage: Storage });

export default upload;
