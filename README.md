MidiPatch is a scriptable modular MIDI-synthesizer. The scripting is done in [lua](https://www.lua.org) and it basically exposes [Tonic](https://github.com/TonicAudio/Tonic) as literal as possible to the scripts. Additionally to the MIDI interface, there is a web interface with live editor, a visual rack generated from the code, an oscilloscope and more. The web app is compiled into and directly served by MidiPatch. To connect to it visit http://127.0.0.1:8080/index.html

![Screenshot of the web interface](https://github.com/kallaballa/MidiPatch/raw/master/doc/screen.png "Screenshot of the web interface")

# Usage

    Usage:
      src/midipatch [OPTION...]

      -h, --help           Print help messages
      -m, --midi arg       The indeces of the midi input ports to use. (default: 0)
      -a, --audio arg      The index of the audio output device to use. (default: 0)
      -r, --rate arg       The audio output sample rate. (default: 44100)
      -b, --buffer arg     Number of frames per buffer. (default: 32)
      -o, --offset arg     The control number offset for parameter mapping
                           (default: 52)
      -v, --voices arg     The number of voices to run (default: 8)
      -s, --save arg       The file where current patch settings are stored
                           (default: /tmp/midipatch.save)
      -p, --patchFile arg  The lua patchFile to use for the voices
      -f, --logFile arg    The file to log to (default: /tmp/midipatch.log)

For example to create a midi synthesizer with 3 voices, running the patch "bank.lua", connecting to the second midi port (port index starts at 0) and the first audio port, which automatically maps patch parameters to control numbers, starting at the control number offset (52) in ascending order:
```shell
src/midipatch -m 1 -a 0 -o 52 -v 3 -p bank.lua
 ```
# Example patch

The following code creates a MIDI patch with 3 oscillators, a low pass filter and an ADSR envelope. Every parameter added with "addParameter" will be audomatically exposed to the midi interface except those starting with '_' (those are private). It  closely resembles the Tonic C++ API.

```lua
local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);
local gain = synth:addParameter("Global.Gain", 1.0);
local sine = synth:addParameter("Global.Sine", 1.0);
local square = synth:addParameter("Global.Square", 1.0);
local triangle = synth:addParameter("Global.Triangle", 1.0);
local cutoffFreq = synth:addParameter("Low Pass.Frequency", 1.0);
local cutoffQ = synth:addParameter("Low Pass.Resonance", 1.0);

local voiceFreq = ControlMidiToFreq():input(noteNum)
local tone0 = SineWave():freq(voiceFreq)
local tone1 = SquareWave():freq(voiceFreq)
local tone2 = TriangleWave():freq(voiceFreq)
local mix = ((tone0 * sine) + (tone1 * square) + (tone2 * triangle )) / 3.0 * gain;

local lpf = LPF24():cutoff(FixedValue(5000) * cutoffFreq):Q(FixedValue(20) * cutoffQ)

local env = ADSR()
:attack(synth:addParameter("Envelope.Attack",0.1))
:decay(synth:addParameter("Envelope.Decay", 0 ))
:sustain(synth:addParameter("Envelope.Sustain",1))
:release(synth:addParameter("Envelope.Release",0.1))
:doesSustain(1)
:trigger(gate)

synth:setOutputGen((lpf:input(mix) * env) * (FixedValue(0.02) + noteVelocity * 0.005));
```

# Build

At the moment only Linux and Mac OS X are supported. It has been shown to work on OpenSuse 15.0, Ubuntu Xenial and Mac OS X.

# Instructions for Ubunto Xenial install (tested on a fresh server image):

```shell
# add toolchain repo
sudo apt-get update
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update

# install dependencies
sudo apt-get install git-core build-essential g++-7 zlib1g-dev libasound2-dev liblilv-dev npm libsqlite3-dev
sudo npm install -g inliner
sudo ln -s /usr/bin/nodejs /usr/bin/node

# clone and build
git clone https://github.com/kallaballa/MidiPatch.git
cd MidiPatch
git submodule update --init --recursive
./build_third.sh -j2
make -j2
```

# Instructions for Mac OS X Mojave

```shell
# install/upgrade clang, we need C++17 support
xcode-select --install 

# only required if you don't have brew
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)" < /dev/null 2> /dev/null

# install dependencies
brew install lilv
brew install npm
brew install nasm
brew install sqlite3

# install inliner
npm install -g inliner

# clone and build
git clone https://github.com/kallaballa/MidiPatch.git
cd MidiPatch
git submodule update --init --recursive
./build_third.sh
make CXX=clang++ CC=clang
```

# Building with other Linux audio backends then ALSA

If you want use pulseaudio or jack you can do that by prepending a variable to the make command

## Building with pulse support:
```shell
PULSE=1 make CXX=g++-7 CC=gcc-7
```
## Building with jack support:
```shell
JACK=1 make CXX=g++-7 CC=gcc-7
```
# Cross-compiling
The makefile supports the sysroot flag. 

## Example
```shell
SYSROOT=/arm-linux-sysroot/ make CXX=arm-suse-linux-gnueabi-g++-7 LD=arm-suse-linux-gnueabi-ld
```
