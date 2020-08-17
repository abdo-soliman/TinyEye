import React, { Component } from "react";
import { Alert } from "react-native";
import DeleteMenu from "../../components/DeleteMenu";
import PersonHeader from "../../components/PersonHeader";
import Axios from "axios";
import apiRoutes from "../../core/apiRoutes";
import GallerySwiper from "react-native-gallery-swiper";

export class PersonScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      id: this.props.route.params.id,
      person: {},
    };
  }

  componentDidMount() {
    const { id } = this.props.route.params;
    Axios.post(apiRoutes.persons.show, { id: id })
      .then((response) => {
        const { data } = response;
        this.setState({ person: data });
        this.configHeader();
      })
      .catch((error) => {
        console.log(error);
        Alert.alert("Error", "Can't load data");
      });
    this.configHeader();
  }

  configHeader = () => {
    this.props.navigation.setOptions({
      headerRight: (props) => <DeleteMenu {...props} />,
      headerTitle: (props) => (
        <PersonHeader
          {...props}
          image={this.state.person.images && this.state.person.images[0].iUrl}
          name={this.state.person.name}
        />
      ),
    });
  };

  render() {
    const { person } = this.state;
    let images = [];
    if (person.images) {
      images = person.images.map((image) => {
        return {
          url: image.iUrl,
        };
      });
    }

    return (
      <GallerySwiper style={{ backgroundColor: "white" }} images={images} />
    );
  }
}

export default PersonScreen;
