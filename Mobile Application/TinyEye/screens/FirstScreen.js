import React, { memo } from 'react';
import Background from '../components/Background';
import Logo from '../components/Logo';
import Header from '../components/Header';
import Button from '../components/Button';
import Paragraph from '../components/Paragraph';

const FirstScreen = ({ navigation }) => (
  <Background>
    <Logo />
    <Header>TinyEye</Header>

    <Paragraph>
      Login if you already have an account 
      </Paragraph>
      <Paragraph>
      OR 
      </Paragraph>
      <Paragraph>
      SignUp to create a new one.
    </Paragraph>
    <Button mode="contained" onPress={() => navigation.navigate('Login')}>
      Login
    </Button>
    <Button
      mode="outlined"
      onPress={() => navigation.navigate('Register')}
    >
      Sign Up
    </Button>
  </Background>
);

export default memo(FirstScreen);
