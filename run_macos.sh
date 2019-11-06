#!/bin/bash

export DYLD_LIBRARY_PATH=third/RtAudio:third/RtMidi:PatchScript/third/Tonic/src:PatchScript/src/
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"
src/midipatch $@ &>/tmp/midipatch-boot.log


