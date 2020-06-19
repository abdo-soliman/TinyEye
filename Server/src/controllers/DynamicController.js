import HomeController from "./HomeController";
import AuthController from "./AuthController";

const controllers = {
  HomeController,
  AuthController
};

class DynamicController {
  constructor(controllerName, opts) {
    return new controllers[controllerName](opts);
  }
}

export default DynamicController;
