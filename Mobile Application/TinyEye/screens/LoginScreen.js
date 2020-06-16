import React, { Component } from "react";
import { StyleSheet } from "react-native";
import { Container, Text, Button } from "native-base";
import { connect } from "react-redux";

export class LoginScreen extends Component {
  login = () => {
    this.props.setLogin();
  };
  render() {
    const { navigation } = this.props;
    return (
      <Container style={styles.container}>
        <Text>Open up App.js to start working on your app!</Text>
        <Button onPress={this.login}>
          <Text>Go to Main!</Text>
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
    setLogin: () => {
      dispatch({
        type: "SET_USER_LOGIN",
        payload: null,
      });
    },
  };
};

export default connect(null, mapDispatchToProps)(LoginScreen);
