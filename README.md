MidiPatch is a scriptable modular synthesizer. The scripting is done in [lua](https://www.lua.org) and it basically exposes [Tonic](https://github.com/TonicAudio/Tonic) as literal as possible to the scripts. At the moment that is used in a simple midi synthesizer implementation, but with a little effort it can be used in a lot of scenarios (like a standalone syntesizer based on patches or in combination with a sequencer).

# Usage

    Usage:
      src/midipatch [OPTION...]

      -h, --help           Print help messages
      -m, --midi arg       The index of the midi input device to use. (default: 0)
      -a, --audio arg      The index of the audio output device to use. (default: 0)
      -r, --rate arg       The audio output sample rate. (default: 44100)
      -b, --buffer arg     Number of frames per buffer. (default: 32)
      -l, --lcd arg        The tty file for the LCD display.
      -o, --offset arg     The control number offset for parameter mapping
                           (default: 52)
      -v, --voices arg     The number of voices to run (default: 8)
      -s, --save arg       The file where current patch settings are stored
                           (default: /tmp/midipatch.save)
      -p, --patchFile arg  The lua patchFile to use for the voices
      -f, --logFile arg    The file to log to (default: /tmp/midipatch.log)

For example to create a midi synthesizer with 3 voices, connecting to the second midi port (port index starts at 0), which automatically maps patch parameters to control numbers, starting at the control number offset in ascending order:

    src/midipatch -m 0 -a 0 -o 52 -v 8 -p /home/elchaschab/devel/MidiPatch/bank.lua -f log
 
Note, that you can use a different patch per voice.

# Example patch

The following code creates a MIDI patch with a square wave tone and an ADSR envelope. Every parameter added with "addParameter" will be audomatically exposed to the midi interface except those starting with '_' (those are private). It  closely resembles the Tonic C++ API.

    local noteNum = synth:addParameter("_polyNote", 0.0);
    local gate = synth:addParameter("_polyGate", 0.0);
    local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
    local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);
    
    local voiceFreq = ControlMidiToFreq():input(noteNum)
    local tone = SquareWave():freq(voiceFreq)
    local fv = FixedValue(0.02)
    local env = ADSR()
        :attack(synth:addParameter("attack",0.04))
        :decay(synth:addParameter("decay", 0.1 ))
        :sustain(synth:addParameter("sustain",0.8))
        :release(synth:addParameter("release",0.6))
        :doesSustain(1)
        :trigger(gate);

    synth:setOutputGen((tone * env) * (fv + noteVelocity * 0.005));

# Build

At the moment only linux is supported but it shouldn't be hard to port it to other platforms.

First install inliner:

    sudo npm install -g inliner

Dependencies: -lasound -llua -ldl -llilv-0 

- libdl
- libasound (ALSA)
- liblilv-0

After you installed the dependencies all you have to do is type

    ./build_third.sh
    make

