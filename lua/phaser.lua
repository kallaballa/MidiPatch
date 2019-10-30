local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);

local voiceFreq = ControlMidiToFreq():input(noteNum)
local tone = (SineWave():freq(voiceFreq) * 0.8) + (SquareWaveBL():freq(voiceFreq) * 0.2)

local env = ADSR()
:attack(synth:addParameter("_attack",0.1))
:decay(synth:addParameter("_decay", 0 ))
:sustain(synth:addParameter("_sustain",1))
:release(synth:addParameter("_release",0.1))
:doesSustain(1)
:trigger(gate);

local phaser = LV2Plugin("http://calf.sourceforge.net/plugins/Reverb")
phaser:input(tone);

for k, v in pairs(phaser:getControlNames()) do
	local cp = synth:addParameter(v,0.5)
	phaser:setControl(v, cp)
end

synth:setOutputGen((phaser * env) * (FixedValue(0.02) + noteVelocity * 0.005));
