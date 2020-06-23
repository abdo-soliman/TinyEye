import React, { Component } from "react";
import { StyleSheet, FlatList, View } from "react-native";
import { connect } from "react-redux";
import { removeToken } from "../core/utils";
import PersonCard from "../components/PersonCard";
import { FAB } from "react-native-paper";
import { theme } from "../core/theme";

export class PersonsScreen extends Component {
  logout = async () => {
    await removeToken();
    this.props.setLogout();
  };
  render() {
    const array = [
      {
        key: "1",
        type: "known",
        image: "https://picsum.photos/id/1011/200/300",
      },
      {
        key: "2",
        type: "known",
        image: "https://picsum.photos/id/1011/200/300",
      },
      {
        key: "3",
        type: "known",
        image: "https://picsum.photos/id/1011/200/300",
      },
      {
        key: "4",
        type: "known",
        image: "https://picsum.photos/id/1011/200/300",
      },
      {
        key: "5",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
      {
        key: "6",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
      {
        key: "7",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
      {
        key: "8",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
      {
        key: "9",
        type: "unknown",
        image: "https://picsum.photos/id/1005/200/300",
      },
    ];
    const { navigation } = this.props;
    return (
      <View
        style={{
          flex: 1,
          marginTop: 80,
          marginBottom: 20,
          alignItems: "center",
        }}
      >
        <FlatList
          style={{ width: "100%" }}
          data={array}
          renderItem={({ item }) => (
            <PersonCard image={item.image} type={item.type} />
          )}
          keyExtractor={(item) => item.key}
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

const mapDispatchToProps = (dispatch) => {
  return {
    setLogout: () => {
      dispatch({
        type: "SET_USER_LOGOUT",
        payload: null,
      });
    },
  };
};

export default connect(null, mapDispatchToProps)(PersonsScreen);
