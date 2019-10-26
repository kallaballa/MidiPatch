local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);

local voiceFreq = ControlMidiToFreq():input(noteNum)
local blend = synth:addParameter("blend"):min(0):max(1);
local freqSweep = SineWave():freq(voiceFreq);
local smoothBlend = blend:smoothed(0.05);
local output = (SquareWave():freq(voiceFreq) * (FixedValue(1.0) - smoothBlend)) + (SquareWaveBL():freq(voiceFreq) * smoothBlend);
local env = ADSR()
        :attack(synth:addParameter("attack",0.04))
        :decay(synth:addParameter("decay", 0.1 ))
        :sustain(synth:addParameter("sustain",0.8))
        :release(synth:addParameter("release",0.6))
        :doesSustain(1)
        :trigger(gate);

synth:setOutputGen((output * env) * (FixedValue(0.02) + noteVelocity * 0.005));

