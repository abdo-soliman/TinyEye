#!/bin/bash

# for this script to run properly both LIB_CAFFE_PATH and LIB_TORCH_PATH variables must be exported in your .bashrc

usage() {
    echo "tinyeye server module installer
parameters:
        -i      --install-path  path for install tinyeye-server_module  DEFAULT ~/tinyeye-server
        -t      --threads       number of threads for building          DEFAULT 4
        -h      --help          print help"
}

installPath="$HOME/tinyeye-server"
numThreads=4

while [ "$1" != "" ]; do
    case $1 in
    -i | --install-path)
        shift
        installPath=$1
        ;;
    -t | --threads)
        shift
        numThreads=$1
        ;;
    -h | --help)
        usage
        exit
        ;;
    *)
        usage
        exit 1
        ;;
    esac
    shift
done

if [ -d "build" ]; then
    rm -rf build
fi
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$installPath ..
make install -j$numThreads

echo "# added by tinyeye-server installer" >> ~/.bashrc
echo "export LD_LIBRARY_PATH=\"\$LIB_CAFFE_PATH/lib:\$LIB_TORCH_PATH/lib:$installPath/lib:\$LD_LIBRARY_PATH\"" >> ~/.bashrc
echo "alias tinyModelBuilder=\"$installPath/bin/tinyeye-server_module 2> /dev/null --server-log-path $HOME/tinyeye-server.log --mtcnn-models-dir $installPath/models/mtcnn --mfn-model-path $installPath/models/mobilefacenet.pt\"" >> ~/.bashrc
source ~/.bashrc

rm -rf build
