import HomeController from "./HomeController";
import AuthController from "./AuthController";
import UserController from "./UserController";
import ModelController from "./ModelController";
import ImageController from "./ImageController";
import HumanController from "./HumanController";
import LogController from "./LogController";

const controllers = {
  HomeController,
  AuthController,
  UserController,
  ModelController,
  ImageController,
  HumanController,
  LogController,
};

class DynamicController {
  constructor(controllerName, opts) {
    return new controllers[controllerName](opts);
  }
}

export default DynamicController;
