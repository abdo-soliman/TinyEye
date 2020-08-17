import React, { useState } from "react";
import { StyleSheet, View } from "react-native";
import { Avatar, IconButton, Menu, List } from "react-native-paper";
import IconBadge from "react-native-icon-badge";
import moment from "moment";

const NotificationCard = (props) => {
  const [visible, setVisible] = useState(false);

  const _openMenu = () => {
    setVisible(true);
  };

  const _closeMenu = () => setVisible(false);

  const image = props.image;
  const symbol = props.type === "known" ? "account-check" : "incognito";
  const { onDelete, onMarkAsRead, id, readAt } = props;
  return (
    <List.Item
      style={!readAt ? styles.unread : {}}
      title={props.type === "known" ? props.name : "Unknown"}
      description={moment(props.time).format("dddd, MMMM Do YYYY, h:mm:ss a")}
      onPress={() => props.onPress(props.id)}
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
                style={{ margin: 6, color: "white" }}
              />
            }
            BadgeElement={
              <Avatar.Icon
                {...props}
                color="white"
                style={{ marginLeft: 0 }}
                size={25}
                icon={symbol}
              />
            }
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
          {!readAt ? (
            <Menu.Item onPress={() => onMarkAsRead(id)} title="Mark as Read" />
          ) : null}
          <Menu.Item onPress={() => onDelete(id)} title="Delete" />
        </Menu>
      )}
    />
  );
};

const styles = StyleSheet.create({
  unread: {
    backgroundColor: "lightgray",
  },
});

export default NotificationCard;
