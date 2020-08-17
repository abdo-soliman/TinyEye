import React from "react";
import { ImageBackground, StyleSheet, ScrollView } from "react-native";

const Background = ({ children }) => (
  <ImageBackground
    source={require("../assets/background_dot.png")}
    resizeMode="repeat"
    style={styles.background}
  >
    <ScrollView style={styles.container} behavior="padding">
      {children}
    </ScrollView>
  </ImageBackground>
);

const styles = StyleSheet.create({
  background: {
    flex: 1,
    width: "100%",
  },
  container: {
    padding: 5,
    width: "100%",
  },
});

export default Background;
