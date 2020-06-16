import React from "react";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import HomeScreen from "../screens/HomeScreen";
import UsersScreen from "../screens/UsersScreen";
import { createStackNavigator } from "@react-navigation/stack";
import { Icon } from "native-base";

const Tab = createBottomTabNavigator();
const Stack = createStackNavigator();

const HomeStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Home" component={HomeScreen} />
    </Stack.Navigator>
  );
};

const UsersStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Users" component={UsersScreen} />
    </Stack.Navigator>
  );
};

const MainTabs = () => {
  return (
    <Tab.Navigator>
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
