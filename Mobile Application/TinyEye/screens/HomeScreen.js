import React, { Component } from "react";
import { StyleSheet, Text, View } from "react-native";
import { connect } from "react-redux";
import Background from "../components/Background";
import { theme } from "../core/theme";
import { Card, Avatar, IconButton, Badge } from "react-native-paper";
import IconBadge from "react-native-icon-badge";
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
      <Background>
        <View style={{ marginTop: 80, marginBottom: 20, alignItems: "center" }}>
          {array.map((item) => (
            <NotificationCard
              key={item.key}
              image={item.image}
              type={item.type}
            />
          ))}
        </View>
      </Background>
    );
  }
}

const styles = StyleSheet.create({});
export default HomeScreen;
