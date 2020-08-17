import { combineReducers } from "redux";
import AuthReducer from "./AuthReducer";

const AllReducers = combineReducers({
  Auth: AuthReducer,
});

export default AllReducers;
