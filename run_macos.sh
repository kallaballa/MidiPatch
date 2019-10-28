#!/bin/bash

DYLD_LIBRARY_PATH=third/Tonic/src/:third/RtAudio:third/RtMidi: src/farts -a 1 -m 1 --rate 44100 lua/sin.lua lua/sin.lua lua/sin.lua lua/sin.lua lua/sin.lua lua/sin.lua
