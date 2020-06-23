import React, { useState } from "react";
import { StyleSheet, View } from "react-native";
import { Card, Avatar, IconButton, Menu, List } from "react-native-paper";
import { TouchableOpacity } from "react-native-gesture-handler";

const PersonCard = (props) => {
  const [visible, setVisible] = useState(false);

  const _openMenu = () => {
    console.log("pressed");
    setVisible(true);
  };

  const _closeMenu = () => setVisible(false);

  const image = props.image;
  return (
    <List.Item
      title="First Item"
      onPress={() => console.log("card")}
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
      right={(props) => (
        <Menu
          visible={visible}
          onDismiss={_closeMenu}
          anchor={
            <IconButton {...props} onPress={_openMenu} icon="dots-vertical" />
          }
        >
          <Menu.Item onPress={() => {}} title="Delete" />
        </Menu>
      )}
    />
  );
};

export default PersonCard;

const styles = StyleSheet.create({});
