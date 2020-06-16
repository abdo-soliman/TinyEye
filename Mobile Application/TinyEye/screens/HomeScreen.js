import React, { Component } from "react";
import { StyleSheet } from "react-native";
import { Container, Text } from "native-base";

export class HomeScreen extends Component {
  render() {
    return (
      <Container style={styles.container}>
        <Text>Open up App.js to start working on your app!</Text>
      </Container>
    );
  }
}
const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
});
export default HomeScreen;
