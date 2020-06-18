import express from "express";
const router = express.Router();
import DynamicController from "../controllers/DynamicController";
import { indexRequest } from "../requests/HomeRequest";

class Route {
  static get = (route, callback, validation = []) => {
    const controller = callback.split("@");
    const controllerClass = new DynamicController(controller[0]);
    const method = controller[1];
    router.get(route, validation, controllerClass[method]);
  };

  static post = (route, callback, validation = []) => {
    const controller = callback.split("@");
    const controllerClass = new DynamicController(controller[0]);
    const method = controller[1];
    router.post(route, validation, controllerClass[method]);
  };
}

Route.get("/", "HomeController@index", indexRequest);
Route.get("/about", "HomeController@about");

export default router;
