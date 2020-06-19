import express from "express";
const router = express.Router();
import DynamicController from "../controllers/DynamicController";
import { indexRequest, aboutRequest } from "../requests/HomeRequest";

class Route {
  static get = (route, callback, middlewares = []) => {
    const controller = callback.split("@");
    const controllerClass = new DynamicController(controller[0]);
    const method = controller[1];
    router.get(route, middlewares, controllerClass[method]);
  };

  static post = (route, callback, middlewares = []) => {
    const controller = callback.split("@");
    const controllerClass = new DynamicController(controller[0]);
    const method = controller[1];
    router.post(route, middlewares, controllerClass[method]);
  };
}

Route.get("/", "HomeController@index", indexRequest);
Route.get("/about", "HomeController@about", aboutRequest);

export default router;
