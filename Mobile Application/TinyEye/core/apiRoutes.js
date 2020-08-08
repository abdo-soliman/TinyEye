export default {
  auth: {
    register: "/register",
    login: "/login",
    me: "/me",
  },
  persons: {
    index: "humans",
    show: "humans/show",
    add: "model/register",
    delete: "/remove/human",
  },
  model: {
    train: "/create/model",
  },
  user: {
    updateToken: "update/notification/token",
  },
  log: {
    index: "log",
    show: "log/show",
    markAsRead: "log/markasread",
    delete: "log/delete",
  },
};
