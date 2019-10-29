cd third/Tonic/
make clean
make -j8 
cd ../lua-5.3.5/
make clean
make -j8 posix
cd ../cxxopts-2.2.0/
mkdir -p build
cd build
cmake ..
make clean
make -j8


