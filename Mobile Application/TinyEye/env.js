export default {
  app: {
    name: "TinyEye",
  },
  api: {
    url:
      process.env.NODE_ENV === "production" ? "" : "http://192.168.1.10:3000/",
    devUrl: "http://192.168.1.10:3000/",
    testUrl: "",
    productionUrl: "",
  },
};
