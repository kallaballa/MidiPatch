local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);

local voiceFreq = ControlMidiToFreq():input(noteNum)
local tone = ((SineWave():freq(voiceFreq) * 0.3) * TriangleWave():freq(voiceFreq)) * 2
local fv = FixedValue(0.02)
   local env = ADSR()
        :attack(synth:addParameter("attack",0.1))
        :decay(synth:addParameter("decay", 0 ))
        :sustain(synth:addParameter("sustain",1))
        :release(synth:addParameter("release",0.1))
        :doesSustain(1)
        :trigger(gate);

synth:setOutputGen((tone * env) * (fv + noteVelocity * 0.005));
