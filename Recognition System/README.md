# TinyEye - Recognition System
A C++ project to run face recognition pipeline on an ARM-based single-board computer.

## Build
To build this project you need CMake and OpenCV (minimum version 3.2) as main dependencies.

### Main dependencies - CMake
To install cmake you can run of the following command:
```bash
# for Debian based operating systems
sudo apt-get install -y cmake
# for rpm based operating systems
sudo yum install cmake
```
For windows, you can install it from the following link: https://cmake.org/download/

### Main dependencies - OpenCV
The easiest way to install the OpenCV C library is to install the precompiled development library in your operating system repositories. First, you will need to install the following dependencies:
```bash
# if you are using rpm Linux just replace apt or apt-get with yum
sudo apt install -y libgtk-3-dev libcanberra-gtk3-dev
sudo apt install -y libtiff-dev zlib1g-dev
sudo apt install -y libjpeg-dev libpng-dev
sudo apt install -y libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
sudo apt-get install -y libxvidcore-dev libx264-dev
```
After the installation is done you can install the development package of OpenCV using the following command:
```bash
# if you are using rpm Linux just replace apt or apt-get with yum
sudo apt-get install -y libopencv libopencv-dev
```
This will install the C library and its header files.

**Note:** This process doesn't depend on anything Debian specific all the libraries installed are just basic C libraries that you can search and find a suitable version of it for your operating system.

This will install the latest Opencv development library provided by your operating system repositories. We are using Opencv 3.2 earlier version may work but it's recommended and most if not all operating systems repositories have migrated to Opencv 3.X or higher.

If you are using windows you can install any Opencv release from here: https://opencv.org/releases/

If you are using windows or you want to install a higher version of OpenCV or build it from source, It probably won't change anything. Just make sure that the headers and library files could be found by the following CMake function:
```cmake
find_package(OpenCV REQUIRED)
```
Otherwise, you will have to edit the CMake file in the src folder [CMakeLists.txt](./src/CMakeLists.txt "CMake configuration of core library") to link Opencv by yourself.

### DeapLearning dependencies - TensorFlow Lite
In our problem, we can't tell for sure what library will perform better in terms of run time and memory usage. The difference may be insignificant in other applications but since we are using very limited hardware any speed up will help. To solve this issue we decided to use both TensorFlow lite and PyTorch.

To get TensorFlow lite as a C library you will need to compile it from the repo so first clone [TensorFlow](https://github.com/tensorflow/tensorflow) by using the following command:
```bash
git clone --recursive https://github.com/tensorflow/tensorflow.git
```
After you will need to build TensorFlow lite you will to run 2 scripts to download the dependencies and build the library. Both scripts are available in the repo and you can run them from repo root folder only using the following commands:
```bash
cd tensorflow
# This scripts currently works only on the master the latest release by the time of making this project the script doesn't work
./tensorflow/lite/tools/make/download_dependencies.sh

# to build you have many scripts in the directory tensorflow/lite/tools/make for different architectures for our case use a raspberry pi for deployment and ubuntu for development so we used only the following scripts

./tensorflow/lite/tools/make/build_lib.sh # for Ubuntu
./tensorflow/lite/tools/make/build_rpi_lib.sh # for Raspbian
```
The compiled library will be then generated in the following directory: tensorflow/lite/tools/make/gen . You will find a folder with the name of of you architecute containing the library files.

We still need the header files to be able to use the library in our project. Unfortunately this we will have to do manually because TensorFlow doesn't provide the header files in the generated library files. If you run the following commands in the root directory of the library it should do the job:
```bash
# 1- create a destination directory that will contain all the library and header files
mkdir ~/libtenroflow # the name libtensorflow is arbitrary you can name the directory whatever you want

# 2- now copy the library files from gen directory to the library folder we just created.
# Note: the gen folder name may change depending on what is the architecture you compiled for in our case the folder name was linux_x86_64 so it's better to check the name out before running the commands.
cp -R tensorflow/lite/tools/make/gen/linux_x86_64/* ~/libtenroflow

# 3- create the include folder
mkdir ~/libtensorflow/include

# 4- copy include dirs of flatbuffers, fp16 and absl which you will find in the downloads folder
cp -R tensorflow/lite/tools/make/downloads/flatbuffers/include/* ~/libtensorflow/include/flatbuffers
cp -R tensorflow/lite/tools/make/downloads/fp16/include/* ~/libtensorflow/include
cp -R tensorflow/lite/tools/make/downloads/absl/absl ~/libtensorflow/include

# 5- copy eigen headers from third_party folder
mkdir -p ~/librensorflow/include/tensorflow/third_party
cp -R third_party/eigen3 ~/libtensorflow/include/tensorflow/third_party

# 6- copy lite directory to include dirs
cp -R tensorflow/lite ~/libtensorflow/include/tensorflow

# 7- remove non header files from eigen3 and lite directory
cd ~/libtensorflow/include/tensorflow
find . -not \( -name "*.h" -o -name "*.hpp" -o -name "*.hh" \) -type f -delete # this command will find any file that doesn't have the extentions .h, .hpp and .hh then delete it
```

We built TensorFlow Lite binaries and headers for Ubuntu 18.04 running on **Intel® Core™ i7-6500U** which you can find [here](https://drive.google.com/open?id=1Vdxrkl4r0162VDVFOle0hMo5jpywR_Tw). We also built it for **Raspberry Pi 3 Model B+** running **Raspbian Buster** which you can find [here](https://drive.google.com/open?id=18uwj4YuNdolXqaPbQyUPJoUDWAI2UmR6).

### DeapLearning dependencies - PyTorch
Pytorch installation is a much easier problem as PyTorch does provide the library which is called **libtorch** which you can easily download from PyTorch official website from the following link: https://pytorch.org/get-started/locally/

Choose your operating system, CUDA version if you are using CUDA and choose from the language choose C++/Java. You will be provided with two links:
1. Pre-cxx11 ABI
2. cxx11 ABI

choose the cxx11 link and download it at the time of creating this project the current release version of PyTorch is 1.4.0 and we use the None CUDA version which could be downloaded from this link: https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.4.0%2Bcpu.zip

Unzip the file you downloaded and you will find a directory named libtorch this is the directory that contains all the library files and header files.

If you are using arm devices there is no pre-compiled version of libtorch so you will have to compile it from source for this you will need to follow the instructions in the following link: https://github.com/pytorch/pytorch#from-source

In our case, we are using a **Raspberry Pi 3 Model B+** and you can find the compiled libtorch files that we are using [here](https://drive.google.com/open?id=1OejAUHTUEpBF6IYmnq-UL6sp4Ww_Pus3)

### Building on ARM Devices - Missing Libraries
If you are building on an ARM device you will get an error of some missing libraries because there is always a missing dependency. So in the scripts folder, you will find the script [rpi-dependancies.sh](./scripts/rpi-dependencies.sh "Script to install possible missing libraries") run the script and it will install a bunch of libraries that may solve the compilation errors you will face. We have tested this a **Raspberry Pi 3 Model B+** running **Raspbian Buster** but it will probably work on any Debian based operating system without an issue.

### Building source code
Once the dependencies and libraries are configured correctly the rest is just a CMake project. So we just need to create a build directory configure some CMake options and build the following script should explain everything
```bash
mkdir build & cd build # create a build directory

# build using tensorflow
cmake .. -DCMAKE_BUILD_TYPE=Release -DLIB_TENSORFLOW_PATH=/path/to/libtensorflow/we/created/above

# build using PyTorch
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_PYTORCH=ON -DLIB_TORCH_PATH=/path/to/libtorch/we/created/above
```
You can change the value of DLIB_TORCH_PATH and LIB_TENSORFLOW_PATH in the [CMakeLists.txt](./CMakeLists.txt "Project CMake configuration file") inline 11 and 12 respectively. to the path of the folders libtorch and libtensorflow instead of passing them every time in compilation.

## Pipeline
We are trying to build the following Face Recognition Pipeline in C++:

![alt text](../Server/Model%20Builder/docs/face_recognition_pipeline.png "Face Recognition Pipeline")

You can see a full explanation of the pipeline different phases [here](../Server/Model%20Builder/README.md#Training)
