import React, { Component } from "react";
import { View, Alert } from "react-native";
import Header from "../../components/Header";
import TextInput from "../../components/TextInput";
import Button from "../../components/Button";
import Axios from "axios";
import apiRoutes from "../../core/apiRoutes";
import { nameValidator } from "../../core/utils";

export class AddPersonNameScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      images: this.props.route.params.images,
      name: {
        value: "",
        error: "",
      },
    };
  }

  componentDidMount() {
    // console.log(this.props.route.params.images);
  }

  handleInputChange = (state) => (value) => {
    this.setState({
      [state]: { value: value, error: "" },
    });
  };

  createFormData = (images, body) => {
    const data = new FormData();

    // data.append("images", {
    //   name: images.fileName,
    //   type: images.type,
    //   uri:
    //     Platform.OS === "android" ? images.uri : images.uri.replace("file://", "")
    // });
    for (let index = 0; index < images.length; index++) {
      const image = images[index];
      data.append("images", {
        uri: image,
        name: `image_${Date.now()}.jpg`,
        type: "image/jpg",
      });
    }

    Object.keys(body).forEach((key) => {
      data.append(key, body[key]);
    });

    return data;
  };

  addAndGoBack = () => {
    const { name, images } = this.state;
    const nameError = nameValidator(name.value);

    if (nameError) {
      this.setState({
        name: { ...name, error: nameError },
      });
      return;
    }

    Axios.post(
      apiRoutes.persons.add,
      this.createFormData(images, { name: name.value }),
      {
        headers: {
          "Content-Type": "multipart/form-data",
        },
      }
    )
      .then((response) => {
        const { data } = response;
        Alert.alert("Sucess", data.msg);
        this.props.navigation.navigate("Persons");
      })
      .catch((error) => {
        // const { data } = error.response;
        Alert.alert("Error", "something wrong happened");
      });
    // this.props.navigation.navigate("Persons");
  };

  render() {
    const { name } = this.state;
    return (
      <View style={{ flex: 1, alignItems: "center", justifyContent: "center" }}>
        <Header>Add name</Header>

        <TextInput
          label="Name"
          returnKeyType="next"
          value={name.value}
          onChangeText={this.handleInputChange("name")}
          error={!!name.error}
          errorText={name.error}
        />

        <Button mode="contained" onPress={this.addAndGoBack}>
          Add person and back to add another one
        </Button>
        <Button mode="outlined" onPress={() => {}}>
          Add person and finish
        </Button>
      </View>
    );
  }
}

export default AddPersonNameScreen;
