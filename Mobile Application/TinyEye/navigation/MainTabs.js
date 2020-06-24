import React from "react";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import HomeScreen from "../screens/HomeScreen";
import PersonsScreen from "../screens/persons/PersonsScreen";
import { createStackNavigator } from "@react-navigation/stack";
import { Icon } from "native-base";
import { theme } from "../core/theme";
import ProfileScreen from "../screens/ProfileScreen";
import PersonScreen from "../screens/persons/PersonScreen";
import LogoHeader from "../components/LogoHeader";
import AddPersonScreen from "../screens/persons/AddPersonScreen";
import AddPersonNameScreen from "../screens/persons/AddPersonNameScreen";

const Tab = createBottomTabNavigator();
const Stack = createStackNavigator();

const HomeStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen
        name="Home"
        component={HomeScreen}
        options={{ headerTitle: (props) => <LogoHeader /> }}
      />
    </Stack.Navigator>
  );
};

const PersonsStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Persons" component={PersonsScreen} />
      <Stack.Screen name="Person" component={PersonScreen} />
      <Stack.Screen
        name="AddPerson"
        component={AddPersonScreen}
        options={{ headerTitle: "Add new person" }}
      />
      <Stack.Screen
        name="AddPersonName"
        component={AddPersonNameScreen}
        options={{ headerTitle: "Add person name" }}
      />
    </Stack.Navigator>
  );
};

const ProfileStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen name="Profile" component={ProfileScreen} />
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
