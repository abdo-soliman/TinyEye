import { check } from "express-validator";
import validate from ".";

export const registerRequest = [
  check("email").isEmail(),
  check("name").isLength({ min: 2 }),
  check("password").isLength({ min: 6 }),
  check("board_uuid").isLength({ min: 6 }),
  validate,
];

export const loginRequest = [
  check("email").isEmail(),
  check("password").isLength({ min: 6 }),
  validate,
];
