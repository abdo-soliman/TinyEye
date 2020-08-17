import React from "react";
import { View, Text } from "react-native";
import { List, Avatar } from "react-native-paper";

const LogoHeader = () => {
  return (
    <List.Item
      title="TinyEye"
      left={(props) => (
        <View
          style={{
            flexDirection: "row",
          }}
        >
          <Avatar.Image size={50} source={require("../assets/icon.png")} />
        </View>
      )}
    />
  );
};

export default LogoHeader;
