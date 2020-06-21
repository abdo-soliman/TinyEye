import React from "react";
import { StyleSheet, Text, TouchableOpacity } from "react-native";
import { theme } from "../core/theme";

const Link = (props) => {
  return (
    <TouchableOpacity onPress={props.onPress}>
      <Text style={{ ...styles.link, ...props.style }} {...props}>
        {props.children}
      </Text>
    </TouchableOpacity>
  );
};

export default Link;

const styles = StyleSheet.create({
  link: {
    fontWeight: "bold",
    color: theme.colors.primary,
  },
});
