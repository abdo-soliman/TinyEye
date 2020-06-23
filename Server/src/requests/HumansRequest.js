import { check } from "express-validator";
import validate from ".";
import AuthMiddleware from "../middlewares/AuthMiddleware";

export const createHumanRequest = [
  AuthMiddleware,
  check("name").isLength({ min: 2 }),
  check("images").isArray(),
  validate,
];

export const removeHumanRequest = [
  AuthMiddleware,
  check("humanId").isNumeric(),
  validate,
];
