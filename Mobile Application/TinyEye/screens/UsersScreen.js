import React, { Component } from "react";
import { StyleSheet } from "react-native";
import { Container, Text, Button } from "native-base";
import { connect } from "react-redux";

export class UsersScreen extends Component {
  logout = () => {
    this.props.setLogout();
  };
  render() {
    return (
      <Container style={styles.container}>
        <Text>Users</Text>
        <Button onPress={this.logout} danger>
          <Text>Logout!</Text>
        </Button>
      </Container>
    );
  }
}
const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
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
