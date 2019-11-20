--[[
This example shows how to create an oscillator with multiple waveforms,
an envelope and control parameters for gain, pitch and the envelope, 
driven by MIDI.
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

local gain = synth:addParameter("OSC.Gain", 1.0)
local sine = synth:addParameter("OSC.Sine", 1.0)
local sineP = synth:addParameter("OSC.SinePitch", 1.0)
local square = synth:addParameter("OSC.Square", 1.0)
local squareP = synth:addParameter("OSC.SquarePitch", 1.0)
local triangle = synth:addParameter("OSC.Triangle", 1.0)
local triangleP = synth:addParameter("OSC.TrianglePitch", 1.0)

local voiceFreq = ControlMidiToFreq():input(noteNum)

local osc = (
                (SineWave():freq(voiceFreq * sineP) * sine) +
  				(SquareWave():freq(voiceFreq * squareP) * square) +
  				(TriangleWave():freq(voiceFreq * triangleP) * triangle)
            ) * 0.33 * gain;

local env = ADSR()
	:attack(attack)
	:decay(decay)
	:sustain(sustain)
	:release(release)
	:doesSustain(1)
	:trigger(gate)

local velocity = (FixedValue(0.02) + noteVelocity * 0.005);

synth:setOutputGen(osc * env * velocity * volume);

