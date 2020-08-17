import React from "react";
import { StyleSheet, Text } from "react-native";
import { theme } from "../core/theme";
import { Headline } from "react-native-paper";

const Header = ({ children }) => (
  <Headline style={styles.header}>{children}</Headline>
);

const styles = StyleSheet.create({
  header: {
    fontSize: 26,
    color: theme.colors.primary,
    paddingVertical: 14,
  },
});

export default Header;
