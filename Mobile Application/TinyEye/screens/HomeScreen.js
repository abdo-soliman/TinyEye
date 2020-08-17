import React, { Component } from "react";
import { View, FlatList, RefreshControl, Alert } from "react-native";
import NotificationCard from "../components/NotificationCard";
import Axios from "axios";
import apiRoutes from "../core/apiRoutes";

export class HomeScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      records: [],
      open: false,
      refreshing: false,
    };
  }

  componentDidMount() {
    this.getLog();
  }

  onRefresh = () => {
    this.getLog();
  };

  getLog = () => {
    Axios.get(apiRoutes.log.index)
      .then((response) => {
        const { data } = response;
        this.setState({ records: data });
      })
      .catch((error) => {
        Alert.alert("Error", "Can't load data");
      });
  };

  goToLog = (id) => {
    this.props.navigation.navigate("Log", { id: id });
  };

  delete = (id) => {
    Axios.post(apiRoutes.log.delete, { id: id })
      .then((response) => {
        const records = this.state.records.filter((record) => record.id !== id);
        this.setState({ records: records });
      })
      .catch((error) => {
        Alert.alert("Error", "Something went wrong");
      });
  };

  markAsRead = (id) => {
    Axios.post(apiRoutes.log.markAsRead, { id: id })
      .then((response) => {
        const { data } = response;
        const records = this.state.records.map((record) => {
          return record.id == id ? data : record;
        });
        this.setState({ records: records });
      })
      .catch((error) => {
        Alert.alert("Error", "Something went wrong");
      });
  };

  render() {
    return (
      <View>
        <FlatList
          style={{ width: "100%" }}
          refreshControl={
            <RefreshControl
              refreshing={this.state.refreshing}
              onRefresh={this.onRefresh}
            />
          }
          data={this.state.records}
          renderItem={({ item }) => (
            <NotificationCard
              onPress={this.goToLog}
              onDelete={this.delete}
              onMarkAsRead={this.markAsRead}
              image={item.iUrl}
              type={item.type}
              name={item.personName}
              id={item.id}
              time={item.createdAt}
              readAt={item.readAt}
            />
          )}
          keyExtractor={(item) => item.id.toString()}
        />
      </View>
    );
  }
}

export default HomeScreen;
