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
