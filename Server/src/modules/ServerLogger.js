import ServerLog from "../../models/ServerLog";

class ServerLogger {
  static error = async (data) => {
    await ServerLog.create({
      type: "error",
      data: data,
    });
  };

  static log = async (data) => {
    await ServerLog.create({
      type: "log",
      data: data,
    });
  };
  static modelLog = async (data) => {
    await ServerLog.create({
      type: "model-log",
      data: data,
    });
  };

  static stdError = async (data) => {
    await ServerLog.create({
      type: "std-error",
      data: data,
    });
  };
}

export default ServerLogger;
