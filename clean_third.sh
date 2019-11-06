#!/bin/bash

set -e
set -x 
cd PatchScript
./clean_third.sh
make clean
cd ../third/uSockets-0.3.2
make clean

