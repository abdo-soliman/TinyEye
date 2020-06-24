import React, { Component } from "react";
import { StyleSheet, FlatList, View, Alert } from "react-native";
import { connect } from "react-redux";
import { removeToken } from "../core/utils";
import PersonCard from "../components/PersonCard";
import { FAB } from "react-native-paper";
import { theme } from "../core/theme";
import Axios from "axios";
import apiRoutes from "../core/apiRoutes";

export class PersonsScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      persons: [],
    };
  }

  componentDidMount() {
    this.getPersons();
  }

  getPersons = () => {
    Axios.get(apiRoutes.persons.index)
      .then((response) => {
        const { data } = response;
        this.setState({ persons: data });
      })
      .catch((error) => {
        Alert.alert("Error", "Can't load data");
      });
  };

  goToPerson = (key) => {
    this.props.navigation.navigate("Person", {
      id: key,
    });
  };

  render() {
    return (
      <View
        style={{
          flex: 1,
        }}
      >
        <FlatList
          style={{ width: "100%" }}
          data={this.state.persons}
          renderItem={({ item }) => (
            <PersonCard
              id={item.id}
              image={item.image}
              name={item.name}
              onPress={() => this.goToPerson(item.id)}
            />
          )}
          keyExtractor={(item) => item.id.toString()}
        />
        <FAB
          style={styles.fab}
          icon="plus"
          onPress={() => console.log("Pressed")}
        />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  fab: {
    position: "absolute",
    margin: 16,
    right: 0,
    bottom: 0,
    backgroundColor: theme.colors.primary,
  },
});

export default PersonsScreen;
