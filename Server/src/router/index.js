import express from "express";
const router = express.Router();
import DynamicController from "../controllers/DynamicController";
import { indexRequest } from "../requests/HomeRequest";
import { registerRequest, loginRequest } from "../requests/AuthRequest";
import AuthMiddleware from "../middlewares/AuthMiddleware";

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
Route.post("/register", "AuthController@register", registerRequest);
Route.post("/login", "AuthController@login", loginRequest);
// User Routes
Route.get("/getuser","UserController@getUser",[AuthMiddleware]);
// Route.post("/adduser","UserController@addUser",[AuthMiddleware]);
Route.get("/deleteuser","UserController@deleteUser",[AuthMiddleware]);
Route.post("/updatepassword","UserController@updatePassword",[AuthMiddleware]);
Route.post("/updateemail","UserController@updateEmail",[AuthMiddleware]);



Route.post("/createmodel","ModelController@createModel",[AuthMiddleware]);
Route.post("/modelRegister","HumanController@prepareData",[AuthMiddleware]);
Route.post("/removehuman","HumanController@removeHuman",[AuthMiddleware]);

// Route.post("/modelRegister","UserController@prepareData",DataRequest,[AuthMiddleware]);
export default router;
