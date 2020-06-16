import React from "react";
import { NavigationContainer } from "@react-navigation/native";
import AuthStack from "./AuthStack";
import MainTabs from "./MainTabs";
import { createStackNavigator } from "@react-navigation/stack";
import { connect } from "react-redux";
const Stack = createStackNavigator();

const MainNavigator = (props) => {
  const { loggedIn } = props;
  return (
    <NavigationContainer>
      <Stack.Navigator>
        {!loggedIn ? (
          <Stack.Screen
            name="Auth"
            component={AuthStack}
            options={{ headerShown: false }}
          />
        ) : (
          <Stack.Screen
            name="Main"
            component={MainTabs}
            options={{ headerShown: false }}
          />
        )}
      </Stack.Navigator>
    </NavigationContainer>
  );
};

const mapStateToProps = (state) => {
  return {
    loggedIn: state.Auth.loggedIn,
  };
};

export default connect(mapStateToProps)(MainNavigator);
