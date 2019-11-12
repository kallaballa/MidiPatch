#!/bin/bash

set -e

cd PatchScript/
./build_third.sh $@
make CXX=g++-7 CC=gcc-7 $@
cd ../third/uSockets-0.3.2
make $@
mv uSockets.a libuSockets.a
