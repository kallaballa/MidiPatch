#!/bin/bash

set -e

cd PatchScript/
./build_third.sh $@
make clean
make $@
cd ../third/uSockets-0.3.2
make clean
make $@
mv uSockets.a libuSockets.a
