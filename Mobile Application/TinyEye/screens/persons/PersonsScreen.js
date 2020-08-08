import React, { Component } from "react";
import {
  StyleSheet,
  FlatList,
  View,
  Alert,
  RefreshControl,
} from "react-native";
import PersonCard from "../../components/PersonCard";
import { FAB } from "react-native-paper";
import { theme } from "../../core/theme";
import Axios from "axios";
import apiRoutes from "../../core/apiRoutes";
import PersonsMenu from "../../components/PersonsMenu";

export class PersonsScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      persons: [],
      open: false,
      refreshing: false,
    };
  }

  componentDidMount() {
    this.getPersons();
    this.configHeader();
  }

  configHeader = () => {
    this.props.navigation.setOptions({
      headerRight: (props) => (
        <PersonsMenu {...props} onPress={this.trainModel} />
      ),
    });
  };

  trainModel = () => {
    Axios.post(apiRoutes.model.train)
      .then((response) => {
        Alert.alert("Info", response.data.msg);
      })
      .catch((error) => {
        Alert.alert("Error", "Failed!");
      });
  };

  onRefresh = () => {
    this.getPersons();
  };

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

  deletePerson = (key) => {
    Axios.post(apiRoutes.persons.delete, { humanId: key })
      .then((response) => {
        const persons = this.state.persons.filter(
          (person) => person.id !== key
        );
        this.setState({ persons: persons });
        Alert.alert("Info", response.data.msg);
      })
      .catch((error) => {
        Alert.alert("Error", "Failed!");
      });
  };

  goToAddPerson = () => {
    this.props.navigation.navigate("AddPerson");
  };

  _onStateChange = ({ open }) => this.setState({ open });

  render() {
    const { open } = this.state;
    return (
      <View
        style={{
          flex: 1,
        }}
      >
        <FlatList
          refreshControl={
            <RefreshControl
              refreshing={this.state.refreshing}
              onRefresh={this.onRefresh}
            />
          }
          style={{ width: "100%" }}
          data={this.state.persons}
          renderItem={({ item }) => (
            <PersonCard
              id={item.id}
              image={item.image}
              name={item.name}
              onPress={() => this.goToPerson(item.id)}
              onDelete={() => this.deletePerson(item.id)}
            />
          )}
          keyExtractor={(item) => item.id.toString()}
        />
        <FAB.Group
          open={open}
          fabStyle={{ backgroundColor: theme.colors.primary }}
          icon={open ? "close" : "plus"}
          actions={[
            {
              icon: "camera",
              label: "Camera",
              onPress: this.goToAddPerson,
            },
            {
              icon: "file-image",
              label: "Gallery",
              onPress: () => console.log("Pressed Gallery"),
            },
          ]}
          onStateChange={this._onStateChange}
          onPress={() => {
            if (open) {
              // do something if the speed dial is open
            }
          }}
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
