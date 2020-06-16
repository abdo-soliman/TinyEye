const AuthReducer = (state = {}, actions) => {
  switch (actions.type) {
    case "SET_USER_LOGIN":
      return {
        ...state,
        loggedIn: true,
        //   user: actions.payload,
      };

    case "SET_USER_LOGOUT":
      return { ...state, loggedIn: false, user: {} };

    default:
      return state;
  }
};

export default AuthReducer;
