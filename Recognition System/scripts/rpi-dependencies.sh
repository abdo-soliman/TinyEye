# installing essentials
sudo apt-get install git cmake numactl numactl-devel build-essential
sudo apt-get install tk-dev libncurses5-dev libncursesw5-dev libreadline6-dev libdb5.3-dev libgdbm-dev libsqlite3-dev libssl-dev libbz2-dev libexpat1-dev liblzma-dev zlib1g-dev
sudo apt install ruby-full
sudo apt install openblas-devel blas-devel

# installing openblas, blas and some other libraries that may be needed
sudo apt-get update
sudo apt-get install libnuma-dev
sudo apt install libopencv
sudo apt-get install libgoogle-glog-dev libgflags-dev
sudo apt install libopenblas* libblas* m4
sudo apt install libatlas-base-dev libjasper-dev

# installing Caffe
sudo apt-get update
sudo apt-get install libdlib-dev
sudo apt-get install caffe-cpu
sudo apt-get install libcaffe-cpu
sudo apt-get install libcaffe-cpu1
sudo apt-get install caffe-tools-cpu
sudo apt-get install libcaffe-cpu-dev

# installing boost, protobuf, snappy and hdf5
sudo apt-get install libboost-all-dev
sudo apt-get install libpbihdf-dev
sudo apt-get install -y libprotobuf-dev libleveldb-dev libsnappy-dev libhdf5-serial-dev protobuf-compiler

# installing liblmdb
sudo apt-get install liblmdb0
sudo apt-get install liblmdb-dev
sudo apt-get install -y liblmdb++-dev
