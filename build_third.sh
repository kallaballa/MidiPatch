#!/bin/bash

set -e

cd PatchScript/
./build_third.sh
make clean
make -j8
cd ../third/uSockets-0.3.2
make clean
make -j8
mv uSockets.a libuSockets.a
