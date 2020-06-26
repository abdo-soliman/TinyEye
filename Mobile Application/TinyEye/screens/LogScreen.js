import React, { Component } from "react";
import { Text, View, Alert, Image, Dimensions } from "react-native";
import apiRoutes from "../core/apiRoutes";
import Axios from "axios";
import Background from "../components/Background";
import { Caption, Subheading } from "react-native-paper";
import moment from "moment";

const screenWidth = Math.round(Dimensions.get("window").width);

export class LogScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      id: this.props.route.params.id,
      log: null,
    };
  }
  componentDidMount() {
    this.getLog();
  }

  configHeader = () => {
    const personName = this.state.log.personName
      ? this.state.log.personName
      : "Unknown";
    this.props.navigation.setOptions({
      headerTitle: personName,
    });
  };

  getLog = () => {
    Axios.post(apiRoutes.log.show, { id: this.state.id })
      .then((response) => {
        const { data } = response;
        this.setState({ log: data });
        this.configHeader();
        if (!data.readAt) {
          this.markAsRead();
        }
      })
      .catch((error) => {
        Alert.alert("Error", "Can't Load data");
      });
  };

  markAsRead = () => {
    Axios.post(apiRoutes.log.markAsRead, { id: this.state.id })
      .then((response) => {
        const { data } = response;
        this.setState({ log: data });
      })
      .catch((error) => {});
  };

  render() {
    const { log } = this.state;
    if (!log) {
      return <View />;
    }
    return (
      <Background>
        <View style={{ flexDirection: "row" }}>
          <Subheading style={{ color: "gray" }}>Time: </Subheading>
          <Subheading>
            {moment(log.createdAt).format("dddd, MMMM Do YYYY, h:mm:ss a")}
          </Subheading>
        </View>
        {log.readAt ? (
          <View style={{ flexDirection: "row" }}>
            <Subheading style={{ color: "gray" }}>Viewed at: </Subheading>
            <Subheading>
              {moment(log.readAt).format("dddd, MMMM Do YYYY, h:mm:ss a")}
            </Subheading>
          </View>
        ) : null}
        <View style={{ alignItems: "center" }}>
          <Image
            source={{ uri: log.iUrl }}
            style={{ width: screenWidth - 20, height: 500 }}
          />
        </View>
      </Background>
    );
  }
}

export default LogScreen;
