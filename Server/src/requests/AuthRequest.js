import { check } from "express-validator";

export const registerRequest = [
  check("email").isEmail(),
  check("Name").isLength({ min: 2 }),
  check("password").isLength({ min: 6 }),
  check("board_uuid").isLength({ min: 6 }),
];

export const loginRequest = [];
