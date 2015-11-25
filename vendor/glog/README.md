
how to build glog-0.3.3:

cd src-code
tar -xf gtest-1.7.0.tar.gz
mkdir build
cd build
../glog-0.3.3/configure --prefix=`pwd`/install
make
make install


how to install glog-0.3.3

cp install ../../platform-specific/

