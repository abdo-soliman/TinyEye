import React, { Component } from "react";
import {
  StyleSheet,
  FlatList,
  View,
  Alert,
  RefreshControl,
} from "react-native";
import PersonCard from "../../components/PersonCard";
import {
  FAB,
  Card,
  IconButton,
  Avatar,
  Button,
  Surface,
} from "react-native-paper";
import { theme } from "../../core/theme";
import Axios from "axios";
import apiRoutes from "../../core/apiRoutes";
import PersonsMenu from "../../components/PersonsMenu";

export class PersonsScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      persons: [],
      model: null,
      open: false,
      refreshing: false,
      train: false,
    };
  }

  componentDidMount() {
    const train =
      this.props.route.params && this.props.route.params.train ? true : false;
    this.setState({ train: train });
    this.getPersons();
    this.getModel();
    // this.configHeader();
  }

  configHeader = () => {
    this.props.navigation.setOptions({
      headerRight: (props) => (
        <PersonsMenu {...props} onPress={this.trainModel} />
      ),
    });
  };

  getModel = () => {
    Axios.get(apiRoutes.model.show)
      .then((response) => {
        const { data } = response;
        console.log(data);
        this.setState({ model: data });
      })
      .catch((error) => {
        Alert.alert("Error", "Can't load data");
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
    this.getModel();
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
        if (this.state.model) {
          this.setState({ model: { ...this.state.model, updated: false } });
        }
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
    const { open, model, train } = this.state;
    return (
      <View
        style={{
          flex: 1,
        }}
      >
        {(!model || !model.updated || train) && (
          <Surface style={{ elevation: 10 }}>
            <Card.Title
              title="Model is not updated"
              subtitle="Press train to update it"
              style={{ backgroundColor: "red", color: "white" }}
              titleStyle={{ color: "white" }}
              subtitleStyle={{ color: "white" }}
              right={(props) => (
                <Surface style={{ elevation: 10, marginRight: 10 }}>
                  <Button color="red" {...props} onPress={this.trainModel}>
                    Train
                  </Button>
                </Surface>
              )}
            />
          </Surface>
        )}
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
            // {
            //   icon: "file-image",
            //   label: "Gallery",
            //   onPress: () => console.log("Pressed Gallery"),
            // },
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
