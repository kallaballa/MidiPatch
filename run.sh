export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/elchaschab/devel/Farts/third/Tonic/src/:/home/elchaschab/devel/Farts/third/lua-5.3.5/src/:/home/elchaschab/devel/Farts/third/RtMidi:/home/elchaschab/devel/Farts/third/RtAudio"

ec=42
while [ $ec -eq 42 ]; do
	src/farts $@;
	ec=$?;
	echo "error code: $ec" 
done

