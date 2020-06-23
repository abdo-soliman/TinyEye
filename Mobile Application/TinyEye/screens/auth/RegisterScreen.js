import React, { Component } from "react";
import { View, Text, StyleSheet } from "react-native";
import Background from "../../components/Background";
import Logo from "../../components/Logo";
import Header from "../../components/Header";
import Button from "../../components/Button";
import TextInput from "../../components/TextInput";
import BackButton from "../../components/BackButton";
import { theme } from "../../core/theme";
import {
  emailValidator,
  passwordValidator,
  nameValidator,
} from "../../core/utils";
import Link from "../../components/Link";
import { connect } from "react-redux";

class RegisterScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      name: {
        value: "",
        error: "",
      },
      email: {
        value: "",
        error: "",
      },
      password: {
        value: "",
        error: "",
      },
      board_uuid: {
        value: "",
        error: "",
      },
    };
  }

  handleInputChange = (state) => (value) => {
    this.setState({
      [state]: { value: value, error: "" },
    });
  };

  _onSignUpPressed = () => {
    const { navigation } = this.props;
    const { name, email, password, board_uuid } = this.state;
    const nameError = nameValidator(name.value);
    const boardError = nameValidator(board_uuid.value);
    const emailError = emailValidator(email.value);
    const passwordError = passwordValidator(password.value);

    if (emailError || passwordError || nameError || boardError) {
      this.setState({
        name: { ...name, error: nameError },
        email: { ...email, error: emailError },
        board_uuid: { ...board_uuid, error: boardError },
        password: { ...password, error: passwordError },
      });
      return;
    }

    navigation.navigate("Login");
  };

  render() {
    const { navigation } = this.props;
    const { name, email, password, board_uuid } = this.state;
    return (
      <Background>
        <BackButton navigation={navigation} />
        <View style={{ marginTop: 120, alignItems: "center" }}>
          <Logo />

          <Header>Create Account</Header>

          <TextInput
            label="Name"
            returnKeyType="next"
            value={name.value}
            onChangeText={this.handleInputChange("name")}
            error={!!name.error}
            errorText={name.error}
          />

          <TextInput
            label="Email"
            returnKeyType="next"
            value={email.value}
            onChangeText={this.handleInputChange("email")}
            error={!!email.error}
            errorText={email.error}
            autoCapitalize="none"
            autoCompleteType="email"
            textContentType="emailAddress"
            keyboardType="email-address"
          />

          <TextInput
            label="Board Serial"
            returnKeyType="next"
            value={board_uuid.value}
            onChangeText={this.handleInputChange("board_uuid")}
            error={!!board_uuid.error}
            errorText={board_uuid.error}
          />

          <TextInput
            label="Password"
            returnKeyType="done"
            value={password.value}
            onChangeText={this.handleInputChange("password")}
            error={!!password.error}
            errorText={password.error}
            secureTextEntry
          />

          <Button
            mode="contained"
            onPress={this._onSignUpPressed}
            style={styles.button}
          >
            Sign Up
          </Button>

          <View style={styles.row}>
            <Text style={styles.label}>Already have an account? </Text>
            <Link onPress={() => navigation.navigate("Login")}>Login</Link>
          </View>
        </View>
      </Background>
    );
  }
}

const styles = StyleSheet.create({
  label: {
    color: theme.colors.secondary,
  },
  button: {
    marginTop: 24,
  },
  row: {
    flexDirection: "row",
    marginTop: 4,
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

export default connect(null, mapDispatchToProps)(RegisterScreen);
