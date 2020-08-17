import React, { useState } from "react";
import { Menu, IconButton } from "react-native-paper";

const DeleteMenu = (props) => {
  const [visible, setVisible] = useState(false);

  const _openMenu = () => {
    setVisible(true);
  };

  const _closeMenu = () => setVisible(false);
  return (
    <Menu
      visible={visible}
      onDismiss={_closeMenu}
      anchor={
        <IconButton {...props} onPress={_openMenu} icon="dots-vertical" />
      }
    >
      <Menu.Item onPress={props.onPress} title="Delete" />
    </Menu>
  );
};

export default DeleteMenu;
