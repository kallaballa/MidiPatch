#!/bin/bash

set -e
set -x 
cd PatchScript/
./build_third.sh $@
make $@
cd ../third/uSockets-0.3.2
make
mv uSockets.a libuSockets.a
#cd ../macdylibbundler
#make
