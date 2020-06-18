import HomeController from "./HomeController";

const controllers = {
  HomeController,
};

class DynamicController {
  constructor(controllerName, opts) {
    return new controllers[controllerName](opts);
  }
}

export default DynamicController;
