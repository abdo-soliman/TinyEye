import { check } from "express-validator";

export const indexRequest = [check("username").isEmail()];
