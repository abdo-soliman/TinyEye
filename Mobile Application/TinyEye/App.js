import React from "react";
import { AppLoading } from "expo";
import * as Font from "expo-font";
import { Ionicons } from "@expo/vector-icons";
import MainNavigator from "./navigation";
import store from "./store";
import { Provider as PaperProvider } from "react-native-paper";
import { Provider as StoreProvider } from "react-redux";
import { AsyncStorage, Alert } from "react-native";
import Axios from "axios";
import env from "./env";
import apiRoutes from "./core/apiRoutes";
import { axiosAuth, removeToken } from "./core/utils";
import { Notifications } from "expo";
import * as Permissions from "expo-permissions";
import Constants from "expo-constants";

export default class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      isReady: false,
    };
  }

  async componentDidMount() {
    await Font.loadAsync({
      Roboto: require("native-base/Fonts/Roboto.ttf"),
      Roboto_medium: require("native-base/Fonts/Roboto_medium.ttf"),
      ...Ionicons.font,
    });
  }

  login = (data) => {
    store.dispatch({ type: "SET_USER_LOGIN", payload: data });
  };

  loadApp = async () => {
    Axios.defaults.baseURL = env.api.url;
    const token = await this._getToken();
    if (token) {
      axiosAuth(token);
      Axios.get(apiRoutes.auth.me)
        .then(async (response) => {
          const { data } = response;
          this.login(data);
          await this.registerForPushNotificationsAsync();
        })
        .catch(async (error) => {
          console.log(error);
          if (error && error.response && error.response.data) {
            const { data } = error.response;
            Alert.alert("Error", data.errors[0].msg);
            if (data.errors[0].msg === "Unauthorized") {
              await removeToken();
            }
          } else {
            Alert.alert("Error", "Failed to login");
          }
        });
    }
  };

  registerForPushNotificationsAsync = async () => {
    if (Constants.isDevice) {
      const { status: existingStatus } = await Permissions.getAsync(
        Permissions.NOTIFICATIONS
      );
      let finalStatus = existingStatus;
      if (existingStatus !== "granted") {
        const { status } = await Permissions.askAsync(
          Permissions.NOTIFICATIONS
        );
        finalStatus = status;
      }
      if (finalStatus !== "granted") {
        alert("Failed to get push token for push notification!");
        return;
      }
      const token = await Notifications.getExpoPushTokenAsync();
      if (token) {
        this.updateUserWithNotificationToken(token);
      }
    } else {
      alert("Must use physical device for Push Notifications");
    }

    if (Platform.OS === "android") {
      Notifications.createChannelAndroidAsync("default", {
        name: "default",
        sound: true,
        priority: "max",
        vibrate: [0, 250, 250, 250],
      });
    }
  };

  updateUserWithNotificationToken = (token) => {
    Axios.post(apiRoutes.user.updateToken, { token: token })
      .then((response) => {
        const { data } = response;
        this.login(data);
      })
      .catch((error) => {
        Alert.alert("Error", "Failed to update notification settings");
      });
  };

  _getToken = async () => {
    try {
      return await AsyncStorage.getItem("token");
    } catch (error) {
      return null;
    }
  };

  render() {
    if (!this.state.isReady) {
      return (
        <AppLoading
          startAsync={this.loadApp}
          onFinish={() => this.setState({ isReady: true })}
        />
      );
    }

    return (
      <StoreProvider store={store}>
        <PaperProvider>
          <MainNavigator />
        </PaperProvider>
      </StoreProvider>
    );
  }
}
