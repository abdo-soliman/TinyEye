import React from "react";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import HomeScreen from "../screens/HomeScreen";
import UsersScreen from "../screens/UsersScreen";
import { createStackNavigator } from "@react-navigation/stack";
import { Icon } from "native-base";
import { theme } from "../core/theme";
import { BlurView } from 'expo-blur';
import { StyleSheet } from "react-native";

const Tab = createBottomTabNavigator();
const Stack = createStackNavigator();
const headerOptions = {
  headerTransparent: true,
  headerBackground: () => (
    <BlurView tint="light" intensity={100} style={StyleSheet.absoluteFill}/>
  ) 
};

const HomeStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Home" component={HomeScreen} 
      options={headerOptions}
      />
    </Stack.Navigator>
  );
};

const UsersStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Users" component={UsersScreen} 
      options={headerOptions}
      />
    </Stack.Navigator>
  );
};

const MainTabs = () => {
  return (
    <Tab.Navigator
    tabBarOptions={{
      activeTintColor: theme.colors.primary,
      activeBackgroundColor: theme.colors.background,
      inactiveBackgroundColor: theme.colors.background,
    }}
    >
      <Tab.Screen
        name="Home"
        component={HomeStack}
        options={{
          tabBarIcon: ({ focused, color, size }) => (
            <Icon
              type="AntDesign"
              name="home"
              size={size}
              style={{ color: color }}
            />
          ),
        }}
      />
      <Tab.Screen
        name="Users"
        component={UsersStack}
        options={{
          tabBarIcon: ({ focused, color, size }) => (
            <Icon
              type="AntDesign"
              name="user"
              size={size}
              style={{ color: color }}
            />
          ),
        }}
      />
    </Tab.Navigator>
  );
};

export default MainTabs;
