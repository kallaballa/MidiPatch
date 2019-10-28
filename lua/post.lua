local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);

local gain = synth:addParameter("gain", 1.0)
local vibratoAmount = synth:addParameter("vibratoAmount", 1.0);
local vibratoFreq = synth:addParameter("vibratoFreq", 1.0);
local tremoloAmount =synth:addParameter("tremoloAmount", 1.0);
local tremoloFreq = synth:addParameter("tremoloFreq", 1.0);

local vibrato = SineWave():freq(vibratoFreq * 30.0);
local tremolo = SineWave():freq(tremoloFreq * 10.0);

local voiceFreq = ControlMidiToFreq():input(noteNum)
local tone = TriangleWave():freq((FixedValue(1.0) + (vibrato * (vibratoAmount * 10))) + voiceFreq)
local env = ADSR()
:attack(synth:addParameter("attack",0.1))
:decay(synth:addParameter("decay", 0 ))
:sustain(synth:addParameter("sustain",1))
:release(synth:addParameter("release",0.1))
:doesSustain(1)
:trigger(gate);

synth:setOutputGen((tone * gain * env * ( FixedValue(1.0) + (tremolo * (tremoloAmount * 3)) )) * (FixedValue(0.02) + noteVelocity * 0.005))