import React, { Component } from "react";
import { StyleSheet, Text, View } from "react-native";
import { connect } from "react-redux";
import Background from "../../components/Background";
import Logo from "../../components/Logo";
import Header from "../../components/Header";
import Button from "../../components/Button";
import TextInput from "../../components/TextInput";
import BackButton from "../../components/BackButton";
import { theme } from "../../core/theme";
import { emailValidator, passwordValidator } from "../../core/utils";
import Link from "../../components/Link";

class LoginScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      email: {
        value: "",
        error: "",
      },
      password: {
        value: "",
        error: "",
      },
    };
  }

  login = () => {
    const emailError = emailValidator(this.state.email.value);
    const passwordError = passwordValidator(this.state.password.value);
    if (emailError || passwordError) {
      this.setState({
        email: { value: this.state.email.value, error: emailError },
      });
      this.setState({
        password: { value: this.state.password.value, error: passwordError },
      });
      return;
    }
    this.props.setLogin();
  };

  handleInputChange = (state) => (value) => {
    this.setState({
      [state]: { value: value, error: "" },
    });
  };

  render() {
    const { navigation } = this.props;
    return (
      <Background>
        <BackButton navigation={navigation} />
        <View style={{ marginTop: 120, alignItems: "center" }}>
          <Logo />

          <Header>Welcome back.</Header>

          <TextInput
            label="Email"
            returnKeyType="next"
            value={this.state.email.value}
            onChangeText={this.handleInputChange("email")}
            error={!!this.state.email.error}
            errorText={this.state.email.error}
            autoCapitalize="none"
            autoCompleteType="email"
            textContentType="emailAddress"
            keyboardType="email-address"
          />

          <TextInput
            label="Password"
            returnKeyType="done"
            value={this.state.password.value}
            onChangeText={this.handleInputChange("password")}
            error={!!this.state.password.error}
            errorText={this.state.password.error}
            secureTextEntry
          />

          <Button mode="contained" onPress={this.login}>
            Login
          </Button>

          <View style={styles.row}>
            <Text style={styles.label}>Don’t have an account? </Text>
            <Link onPress={() => navigation.navigate("Register")}>Sign up</Link>
          </View>
        </View>
      </Background>
    );
  }
}
const styles = StyleSheet.create({
  row: {
    flexDirection: "row",
    marginTop: 4,
  },
  label: {
    color: theme.colors.secondary,
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
