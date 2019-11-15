#!/bin/bash

if [ "$OSTYPE" == "linux-gnu" -o "$OSTYPE" == "linux" ]; then
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:PatchScript/src/:PatchScript/third/Tonic/src/:PatchScript/third/lua-5.3.5/src/:third/RtMidi:third/RtAudio"
elif [[ "$OSTYPE" == "darwin"* ]]; then
	export DYLD_LIBRARY_PATH=third/RtAudio:third/RtMidi:PatchScript/third/Tonic/src:PatchScript/src/
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"
PatchScript/src/fsexporter/fsexporter $@ 


