import React, { Component } from "react";
import { Text, View, Dimensions, StyleSheet } from "react-native";
import { Camera } from "expo-camera";
import { Surface, Caption, IconButton } from "react-native-paper";
import { theme } from "../../core/theme";
import Background from "../../components/Background";
import Button from "../../components/Button";

const screenWidth = Math.round(Dimensions.get("window").width);

export class AddPersonScreen extends Component {
  constructor(props) {
    super(props);
    this.state = {
      type: Camera.Constants.Type.front,
      hasPermission: null,
      number: 10,
      images: [],
    };
    this.camera = null;
  }

  async componentDidMount() {
    let { status } = await Camera.requestPermissionsAsync();
    while (status !== "granted") {
      status = await Camera.requestPermissionsAsync().status;
    }
    this.setState({ hasPermission: status === "granted" });
  }

  takePic = async () => {
    const { number } = this.state;
    if (number) {
      const image = await this.camera.takePictureAsync({
        quality: 0.5,
      });
      this.setState({ number: number - 1 });
      this.setState((state) => {
        const images = [...state.images, image.uri];

        return {
          images,
        };
      });
    }
  };

  goToAddName = () => {
    this.props.navigation.navigate("AddPersonName", {
      images: this.state.images,
    });
  };

  render() {
    const { hasPermission, type, number } = this.state;
    if (hasPermission === null) {
      return <View />;
    }
    if (hasPermission === false) {
      return <Text>No access to camera</Text>;
    }
    return (
      <Background>
        <View
          style={{
            flex: 1,
            alignItems: "center",
          }}
        >
          <Text style={styles.number}>{number}</Text>
          <View
            style={{
              marginVertical: 10,
              backgroundColor: "red",
              borderRadius: screenWidth - 20,
              overflow: "hidden",
              borderColor: "white",
              borderWidth: 10,
            }}
          >
            <Surface>
              <Camera
                ratio="1:1"
                ref={(ref) => {
                  this.camera = ref;
                }}
                style={{
                  width: screenWidth - 20,
                  height: screenWidth - 20,
                }}
                type={type}
              />
            </Surface>
          </View>
          {!number ? (
            <Button mode="contained" onPress={this.goToAddName}>
              Next
            </Button>
          ) : (
            <Surface style={{ borderRadius: 1000 }}>
              <IconButton
                size={40}
                style={{ backgroundColor: theme.colors.primary }}
                color={"white"}
                icon="camera-iris"
                onPress={this.takePic}
              />
            </Surface>
          )}
          <Caption>
            Note that you need to center your face in the circle
          </Caption>
          <Caption>
            Quality of the pictures indicates the quality of the recognition
          </Caption>
          <Caption>Try to take the pictures from diffrent angles</Caption>
        </View>
      </Background>
    );
  }
}

const styles = StyleSheet.create({
  number: {
    fontSize: 70,
    color: theme.colors.primary,
  },
});

export default AddPersonScreen;
