--[[
This example shows how to create two oscillator with multiple waveforms, 
a amplitude modulator, an envelope and control parameters for gain,
pitch and the envelope, driven by MIDI.
]]--

local noteNum = synth:addParameter("_polyNote", 0.0)
local gate = synth:addParameter("_polyGate", 0.0)
local noteVelocity = synth:addParameter("_polyVelocity", 0.0)
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0)

local volume = synth:addParameter("Global.Volume", 0.5)
local attack = synth:addParameter("Global.Attack",0.1)
local decay = synth:addParameter("Global.Decay", 0 )
local sustain = synth:addParameter("Global.Sustain",1)
local release = synth:addParameter("Global.Release",0.1)

local gain0 = synth:addParameter("OSC0.Gain", 1.0)
local sine0 = synth:addParameter("OSC0.Sine", 1.0)
local sineP0 = synth:addParameter("OSC0.SinePitch", 1.0)
local square0 = synth:addParameter("OSC0.Square", 1.0)
local squareP0 = synth:addParameter("OSC0.SquarePitch", 1.0)
local triangle0 = synth:addParameter("OSC0.Triangle", 1.0)
local triangleP0 = synth:addParameter("OSC0.TrianglePitch", 1.0)

local gain1 = synth:addParameter("OSC1.Gain", 1.0)
local sine1 = synth:addParameter("OSC1.Sine", 1.0)
local sineP1 = synth:addParameter("OSC1.SinePitch", 1.0)
local square1 = synth:addParameter("OSC1.Square", 1.0)
local squareP1 = synth:addParameter("OSC1.SquarePitch", 1.0)
local triangle1 = synth:addParameter("OSC1.Triangle", 1.0)
local triangleP1 = synth:addParameter("OSC1.TrianglePitch", 1.0)

local lfoAmpFreq = synth:addParameter("MOD.Amp_Freq", 0.5)
local lfoAmpDry = synth:addParameter("MOD.Amp_Dry", 1.0)
local lfoAmpWet = synth:addParameter("MOD.Amp_Wet", 0.5)
local lfoAmpAmount = synth:addParameter("MOD.Amp_Amount", 0.5)


local voiceFreq = ControlMidiToFreq():input(noteNum)

local lfoAmp = SineWave():freq(lfoAmpFreq * 10)

local osc0Dry = (
  					(SineWave():freq(voiceFreq * sineP0) * sine0) +
  					 (SquareWave():freq(voiceFreq * squareP0) * square0) +
  					 (TriangleWave():freq(voiceFreq * triangleP0) * triangle0)
					) * 0.33 * gain0;

local osc0AmpWet = (osc0Dry * lfoAmp); 

local osc1Dry = (
  					(SineWave():freq(voiceFreq * sineP1) * sine1) +
  					 (SquareWave():freq(voiceFreq * squareP1) * square1) +
  					 (TriangleWave():freq(voiceFreq * triangleP1) * triangle1)
					) * 0.33 * gain1;

local osc1AmpWet = (osc1Dry * (FixedValue(1.0) - lfoAmp)); 


local mix = ((osc0AmpWet * lfoAmpWet) + (osc0Dry * lfoAmpDry) +
  				(osc1AmpWet * lfoAmpWet) + (osc1Dry * lfoAmpDry)) * 0.25;

local env = ADSR()
	:attack(attack)
	:decay(decay)
	:sustain(sustain)
	:release(release)
	:doesSustain(1)
	:trigger(gate)

local velocity = (FixedValue(0.02) + noteVelocity * 0.005)

synth:setOutputGen(mix * env * velocity * volume);
