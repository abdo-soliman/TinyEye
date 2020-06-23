import { check } from "express-validator";
import validate from ".";
import AuthMiddleware from "../middlewares/AuthMiddleware";

export const getUserRequest = [
  AuthMiddleware,
  check("id").isNumeric(),
  validate,
];

export const deleteUserRequest = [
  AuthMiddleware,
  check("id").isNumeric(),
  validate,
];

export const updateUserPasswordRequest = [
  AuthMiddleware,
  check("id").isNumeric(),
  check("password").isLength({ min: 6 }),
  check("passwordConfirmation").isLength({ min: 6 }),
  validate,
];

export const updateUserEmailRequest = [
  AuthMiddleware,
  check("id").isNumeric(),
  check("email").isEmail(),
  validate,
];
