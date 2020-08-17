import { check } from "express-validator";
import AuthMiddleware from "../middlewares/AuthMiddleware";
import validate from ".";

export const indexRequest = [AuthMiddleware, check("username").isEmail(), validate];

export const aboutRequest = [];
