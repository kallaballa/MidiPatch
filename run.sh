export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:third/Tonic/src/:/third/lua-5.3.5/src/:third/RtMidi:third/RtAudio"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"
ec=42
while [ $ec -eq 42 ]; do
	src/farts $@;
	ec=$?;
done

