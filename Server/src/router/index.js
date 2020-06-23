import router, { Route } from "./Route";
import { indexRequest } from "../requests/HomeRequest";
import { registerRequest, loginRequest } from "../requests/AuthRequest";
import {
  createHumanRequest,
  removeHumanRequest,
} from "../requests/HumansRequest";
import AuthMiddleware from "../middlewares/AuthMiddleware";
import {
  getUserRequest,
  deleteUserRequest,
  updateUserPasswordRequest,
  updateUserEmailRequest,
} from "../requests/UsersRequest";

Route.get("/", "HomeController@index", indexRequest);
Route.post("/register", "AuthController@register", registerRequest);
Route.post("/login", "AuthController@login", loginRequest);
Route.get("/me", "AuthController@me", [AuthMiddleware]);
// User Routes
Route.get("/get/user", "UserController@getUser", getUserRequest);
Route.get("/delete/user", "UserController@deleteUser", deleteUserRequest);
Route.post(
  "/update/password",
  "UserController@updatePassword",
  updateUserPasswordRequest
);
Route.post(
  "/update/email",
  "UserController@updateEmail",
  updateUserEmailRequest
);

Route.post("/create/model", "ModelController@createModel", [AuthMiddleware]);
Route.post(
  "/model/register",
  "HumanController@prepareData",
  createHumanRequest
);
Route.post("/remove/human", "HumanController@removeHuman", removeHumanRequest);
Route.get("/humans", "HumanController@index", [AuthMiddleware]);

export default router;
