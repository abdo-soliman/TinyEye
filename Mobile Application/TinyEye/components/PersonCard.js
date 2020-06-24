import React from "react";
import { StyleSheet, View } from "react-native";
import { Avatar, List } from "react-native-paper";
import DeleteMenu from "./DeleteMenu";

const PersonCard = (props) => {
  const { image, id, name } = props;
  return (
    <List.Item
      title={name}
      onPress={props.onPress}
      left={(props) => (
        <View
          style={{
            flexDirection: "row",
            alignItems: "center",
            justifyContent: "center",
          }}
        >
          <Avatar.Image
            size={50}
            source={{ uri: image }}
            style={{ margin: 6 }}
          />
        </View>
      )}
      right={(props) => <DeleteMenu {...props} />}
    />
  );
};

export default PersonCard;

const styles = StyleSheet.create({});
