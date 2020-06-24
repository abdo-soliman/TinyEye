import React, { Component } from "react";
import { View, FlatList } from "react-native";
import NotificationCard from "../components/NotificationCard";

export class HomeScreen extends Component {
  render() {
    const { navigation } = this.props;
    const array = [
      {
        key: "1",
        type: "known",
        image: "https://picsum.photos/id/1011/200/300",
      },
      {
        key: "2",
        type: "known",
        image: "https://picsum.photos/id/1011/200/300",
      },
      {
        key: "3",
        type: "known",
        image: "https://picsum.photos/id/1011/200/300",
      },
      {
        key: "4",
        type: "known",
        image: "https://picsum.photos/id/1011/200/300",
      },
      {
        key: "5",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
      {
        key: "6",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
      {
        key: "7",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
      {
        key: "8",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
      {
        key: "9",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
    ];
    return (
      <View>
        <FlatList
          style={{ width: "100%" }}
          data={array}
          renderItem={({ item }) => (
            <NotificationCard image={item.image} type={item.type} />
          )}
          keyExtractor={(item) => item.key}
        />
      </View>
    );
  }
}

export default HomeScreen;
