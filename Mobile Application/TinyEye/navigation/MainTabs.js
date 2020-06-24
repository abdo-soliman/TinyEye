import React from "react";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import HomeScreen from "../screens/HomeScreen";
import PersonsScreen from "../screens/PersonsScreen";
import { createStackNavigator } from "@react-navigation/stack";
import { Icon } from "native-base";
import { theme } from "../core/theme";
import { BlurView } from "expo-blur";
import { StyleSheet } from "react-native";
import { ProfileScreen } from "../screens/ProfileScreen";
import PersonScreen from "../screens/PersonScreen";

const Tab = createBottomTabNavigator();
const Stack = createStackNavigator();
const headerOptions = {};

const HomeStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen
        name="Home"
        component={HomeScreen}
        options={headerOptions}
      />
    </Stack.Navigator>
  );
};

const PersonsStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen
        name="Persons"
        component={PersonsScreen}
        options={headerOptions}
      />
      <Stack.Screen
        name="Person"
        component={PersonScreen}
        options={headerOptions}
      />
    </Stack.Navigator>
  );
};

const ProfileStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen
        name="Profile"
        component={ProfileScreen}
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
        name="Persons"
        component={PersonsStack}
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
      <Tab.Screen
        name="Profile"
        component={ProfileStack}
        options={{
          tabBarIcon: ({ focused, color, size }) => (
            <Icon
              type="AntDesign"
              name="profile"
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
