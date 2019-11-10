#!/bin/bash

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:PatchScript/src/:PatchScript/third/Tonic/src/:PatchScript/third/lua-5.3.5/src/:third/RtMidi:third/RtAudio"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"
src/midipatch -f /dev/stdout $@

