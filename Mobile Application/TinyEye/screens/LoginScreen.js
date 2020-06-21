import React, { Component, setState } from "react";
import { TouchableOpacity, StyleSheet, Text, View } from "react-native";
// import { Container, Text, Button, View } from "native-base";
import { connect } from "react-redux";
// import { TextInput } from "react-native-gesture-handler";
import Background from '../components/Background';
import Logo from '../components/Logo';
import Header from '../components/Header';
import Button from '../components/Button';
import TextInput from '../components/TextInput';
import BackButton from '../components/BackButton';
import { theme } from '../core/theme';
import { emailValidator, passwordValidator } from '../core/utils';


export class LoginScreen extends Component {
  login = () => {
    const emailError = emailValidator(this.state.email.value);
    const passwordError = passwordValidator(this.state.password.value);
    if (emailError || passwordError) {
      this.setState({email : {value : this.state.email.value, error: emailError} });
      this.setState({password : {value : this.state.password.value, error: passwordError} });
      return;
    }
    this.props.setLogin();
  };

  state={
    email:{
      value:"",
      error:""
    },
    password:{
      value:"",
      error:""
    }
  }

  render() {
    const { navigation } = this.props;
    return (
      <Background>
      <BackButton goBack={() => navigation.navigate('First')} />

      <Logo />

      <Header>Welcome back.</Header>

      <TextInput
        label="Email"
        returnKeyType="next"
        value={this.state.email.value}
        onChangeText={text => this.setState({email : {value : text, error: ''} })}
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
        onChangeText={text => this.setState({password : {value : text, error: ''} })}
        error={!!this.state.password.error}
        errorText={this.state.password.error}
        secureTextEntry
      />

      <View style={styles.forgotPassword}>
        <TouchableOpacity
          onPress={() => navigation.navigate('ForgotPassword')}
        >
          <Text style={styles.label}>Forgot your password?</Text>
        </TouchableOpacity>
      </View>

      <Button mode="contained" onPress={this.login}>
        Login
      </Button>

      <View style={styles.row}>
        <Text style={styles.label}>Don’t have an account? </Text>
        <TouchableOpacity onPress={() => navigation.navigate('Register')}>
          <Text style={styles.link}>Sign up</Text>
        </TouchableOpacity>
      </View>
    </Background>
    );
  }
}
const styles = StyleSheet.create({
  forgotPassword: {
    width: '100%',
    alignItems: 'flex-end',
    marginBottom: 24,
  },
  row: {
    flexDirection: 'row',
    marginTop: 4,
  },
  label: {
    color: theme.colors.secondary,
  },
  link: {
    fontWeight: 'bold',
    color: theme.colors.primary,
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
