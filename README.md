![](https://github.com/kallaballa/MidiPatch/workflows/C%2FC%2B%2B%20CI/badge.svg)


Join the discussion! https://discord.gg/5Ca7vc5
MidiPatch is a novel kind of modular software synthesizer. What makes it so different is that it doesn’t provide a single synthesizer model to make music with. Instead it provides a synthesizer engine that can create all kinds of synthesizers and sounds in the world. A synthesizer model can be created by using a very simple, yet powerful, scripting language: PatchScript. But a scriptable synthesizer is hardly a novelty – What makes it so different is how you script synthesizers. Instead of doing live coding (= directly shaping the signal) you declare a synthesizer by combining modules with a building block system.
It provides the artist with an intergrate development environment (IDE) aiding in the creation of synthesizer models. At the moment the IDE features a code editor, a rack generated from the code, a patch library, a spectrograph, an oscilloscope and a virtual piano. 

![Screenshot of the web interface](https://github.com/kallaballa/MidiPatch/raw/master/doc/screen.png "Screenshot of the web interface")

# Usage

    A scriptable, modular and real-time MIDI synthesizer
    Usage:
      src/midipatch [OPTION...]
    
      -h, --help           Print help messages
      -m, --midi arg       The indeces of the midi input ports to use. (default: 0)
      -a, --audio arg      The index of the audio output port to use. (default: 0)
      -r, --rate arg       The audio output sample rate. (default: 48000)
      -b, --buffer arg     Number of frames per buffer. (default: 512)
      -w, --websocket arg  The port number of the websocket server. (default: 0)
      -o, --offset arg     The control number offset for parameter mapping (default: 52)
      -v, --voices arg     The number of voices to run (default: 8)
      -s, --save arg       The file where current patch settings are stored (default: ~/.patchscript/data/midipatch.sav)
      -p, --patchFile arg  The lua patchFile to use for the voices (default:~/.patchscript/data/midipatch.pat)
      -f, --logFile arg    The file to log to (default: ~/.patchscript/logs//midipatch.log)

For example to create a midi synthesizer with 16 voices, running the patch "bank.lua", connecting to the second midi port (port index starts at 0) and the first audio port, which automatically maps patch parameters to control numbers (starting at the control number offset 52) in ascending order and listening for http/websocket on port 8080:

```shell
midipatch -o 52 -m 1 -a 0 2 -v 16 -w 8080 -p bank.lua
 ```
# Example patch

The following code shows how to generate a sine wave from MIDI events. Every parameter added with "addParameter" will be audomatically exposed to the web and midi interface except those starting with '_' (those are private). It  closely resembles the Tonic C++ API. See the "examples" folder for more advanced usage.

## Short version
```lua

local noteNum = synth:addParameter("_polyNote", 0.0)
local gate = synth:addParameter("_polyGate", 0.0)
local noteVelocity = synth:addParameter("_polyVelocity", 0.0)
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0)
local volume = synth:addParameter("Global.Volume", 0.3);

synth:setOutputGen(
	SineWave():freq(ControlMidiToFreq():input(noteNum)) *
	(FixedValue(0.02) + noteVelocity * 0.005) *
  	ADSR():attack(0.01):decay(0):sustain(1):release(0.01):doesSustain(1):trigger(gate) *
  	volume
);
```

## Long version
```lua
--[[
The MIDI note parameters send by the engine. Note that they
all start with an underscore, which means they are private 
and are not included in the rack.
]]--
local noteNum = synth:addParameter("_polyNote", 0.0)
local gate = synth:addParameter("_polyGate", 0.0)
local noteVelocity = synth:addParameter("_polyVelocity", 0.0)
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0)

--[[
The global volume parameter. Scales the signal at the very end
]]--
local volume = synth:addParameter("Global.Volume", 0.3)

--[[
A ControlGenerator that takes MIDI note numbers as input and
outputs the frequency in Hz.
]]--
local freq = ControlMidiToFreq():input(noteNum)

--[[
The sine wave oscillator generating the actual tone at the
note frequency
]]--
local tone = SineWave():freq(freq);

--[[
This parameters is driven by the note velocity and leads to 
a louder tone with a higher velocity.
]]--
local velocityMod = FixedValue(0.02) + noteVelocity * 0.005;

--[[
we need the envelope for two reasons:
1. to prevent clicking on noteOn and noteOff (hence the slight
   attack and release)
2. So we can use it as a gate. Without it the oscillator would 
   be generating a continous signal
]]--
local envelope = ADSR()
	:attack(0.01)
	:decay(0)
	:sustain(1)
	:release(0.01)
	:doesSustain(1)
	:trigger(gate)

--[[
Attenuate the "tone" (sine wave) by the "velocityMod" (higher 
velocity -> louder tone). Then apply the "envelope" (which in
our case does nearly nothing) and scale the signal by
"volume". 
]]--
local signal = tone * velocityMod * envelope * volume;

--[[
Assign the generator "signal" to the synth.
]]--
synth:setOutputGen(signal);
```

# Build

At the moment only Linux and Mac OS X are supported. It has been shown to work on OpenSuse 15.0, Ubuntu Xenial and Mac OS X.

# Instructions for Ubunto Xenial (tested on a fresh server image):

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
sudo make install
```

# Instructions for Manjaro (Arch Linux):

```shell

# install dependencies
sudo pacman -S sqlite zlib alsa-lib lilv npm make libpulse git gcc lv2
sudo npm install -g inliner

# clone and build
git clone http://github.com/kallaballa/MidiPatch.git
cd MidiPatch/
git submodule update --init --recursive
./build_third.sh -j2
PULSE=1 make -j2

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
make
sudo make install
```

# Building with other Linux audio backends then ALSA

If you want use pulseaudio or jack you can do that by prepending a variable to the make command

## Building with pulse support:
```shell
PULSE=1 make
```
## Building with jack support:
```shell
JACK=1 make
```
# Cross-compiling
The makefile supports the sysroot flag. 

## Example
```shell
SYSROOT=/arm-linux-sysroot/ make CXX=arm-suse-linux-gnueabi-g++-7 LD=arm-suse-linux-gnueabi-ld
```
