local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);
local pitchSine = synth:addParameter("pitchSine", 1.0);
local pitchSquare = synth:addParameter("pitchSquare", 1.0);
local mix = synth:addParameter("mix", 1.0)
local modFreq = synth:addParameter("Modulator Frequency", 1.0);

local tone1 = SineWave()
local tone2 = SquareWaveBL()
local modOsc = SineWave():freq(modFreq * 10);

local voiceFreq = ControlMidiToFreq():input(noteNum)
local attSine = (FixedValue(1.0) - (modOsc * mix)) / 2;
local attSquare = (modOsc * mix) / 2;
local tone = (tone1:freq(voiceFreq * pitchSine) * attSine) + (tone2:freq(voiceFreq * pitchSquare) * attSquare)
local fv = FixedValue(0.02)
   local env = ADSR()
       :attack(synth:addParameter("attack",0.1))
        :decay(synth:addParameter("decay", 0 ))
        :sustain(synth:addParameter("sustain",1))
        :release(synth:addParameter("release",0.1))
        :doesSustain(1)
        :trigger(gate);

synth:setOutputGen((tone * env) * (fv + noteVelocity * 0.005));
