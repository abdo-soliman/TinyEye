import { check } from "express-validator";
import validate from ".";
import AuthMiddleware from "../middlewares/AuthMiddleware";
import upload from "../storage";

export const createHumanRequest = [
  AuthMiddleware,
  upload.array("images"),
  check("name").isLength({ min: 2 }),
  check("images").isArray(),
];

export const removeHumanRequest = [
  AuthMiddleware,
  check("humanId").isNumeric(),
  validate,
];
