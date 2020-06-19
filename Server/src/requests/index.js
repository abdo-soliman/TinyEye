import { validationResult } from "express-validator";

const validate = (req, res) => {
  const errors = validationResult(req);
  if (!errors.isEmpty()) {
    return errors;
  }
  return null;
};

export default validate;
