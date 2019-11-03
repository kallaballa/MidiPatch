#!/bin/bash

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:third/Tonic/src/:/third/lua-5.3.5/src/:third/RtMidi:third/RtAudio"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"
src/farts $@ &>/tmp/farts-boot.log

