import React, { useState } from "react";
import { StyleSheet, Text, View } from "react-native";
import { Card, Avatar, IconButton, Menu, Divider } from "react-native-paper";
import IconBadge from "react-native-icon-badge";

const NotificationCard = (props) => {
  const [visible, setVisible] = useState(false);

  const _openMenu = () => {
    console.log("pressed");
    setVisible(true);
  };

  const _closeMenu = () => setVisible(false);

  const image = props.image;
  const symbol = props.type === "known" ? "account-check" : "incognito";
  return (
    <Card.Title
      title={props.type === "known" ? "Family Member" : "Unknown"}
      subtitle={"Entered your home !!"}
      left={(props) => (
        <View
          style={{
            flexDirection: "row",
            alignItems: "center",
            justifyContent: "center",
          }}
        >
          <IconBadge
            MainElement={
              <Avatar.Image
                size={50}
                source={{ uri: image }}
                style={{ margin: 6 }}
              />
            }
            BadgeElement={<Avatar.Icon {...props} size={25} icon={symbol} />}
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
          <Menu.Item onPress={() => {}} title="Mark as Read" />
          <Menu.Item onPress={() => {}} title="Delete" />
        </Menu>
      )}
    />
  );
};

export default NotificationCard;

const styles = StyleSheet.create({});
