import React, { Component } from "react";
import { StyleSheet } from "react-native";
import { Container, Text, Button, View } from "native-base";
import { connect } from "react-redux";
import Background from "../components/Background";
import { theme } from "../core/theme";

export class UsersScreen extends Component {
  logout = () => {
    this.props.setLogout();
  };
  render() {
    const { navigation } = this.props;
    return (
      <Background>
        <View style={{ marginTop: 120, alignItems: "center" }}>
        <Text>Users</Text>
        <Button onPress={this.logout} danger>
          <Text>Logout!</Text>
        </Button>
        </View>
      </Background>
    );
  }
}
const styles = StyleSheet.create({
  
});

const mapDispatchToProps = (dispatch) => {
  return {
    setLogout: () => {
      dispatch({
        type: "SET_USER_LOGOUT",
        payload: null,
      });
    },
  };
};

export default connect(null, mapDispatchToProps)(UsersScreen);
