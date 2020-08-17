import React from "react";
import { View } from "react-native";
import { Avatar, List } from "react-native-paper";

const PersonHeader = (props) => {
  const { image, name } = props;
  return (
    <List.Item
      title={name}
      left={(props) => (
        <View
          style={{
            flexDirection: "row",
          }}
        >
          <Avatar.Image
            size={50}
            source={{ uri: image }}
            style={{ marginLeft: -20 }}
          />
        </View>
      )}
    />
  );
};

export default PersonHeader;
