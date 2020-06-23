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
import { axiosAuth } from "./core/utils";

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

  loadApp = async () => {
    Axios.defaults.baseURL = env.api.url;
    const token = await this._getToken();
    if (token) {
      axiosAuth(token);
      Axios.get(apiRoutes.auth.me)
        .then((response) => {
          const { data } = response;
          store.dispatch({ type: "SET_USER_LOGIN", payload: data });
        })
        .catch((error) => {
          const { data } = error.response;
          if (data) {
            Alert.alert("Error", data.errors[0].msg);
          } else {
            Alert.alert("Error", "Failed to login");
          }
        });
    }
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
