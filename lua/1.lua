local metro = ControlMetro():bpm(100);
local freq = ControlRandom():trigger(metro):min(0):max(1);
local tone = SquareWaveBL():freq(
                                 freq * 0.25 + 100.0
                                 + 400.0
                                 ) * SineWave():freq(50);
local env = ADSR()
:attack(0.01)
:decay( 0.4 )
:sustain(0)
:release(0)
:doesSustain(0)
:trigger(metro);


local filterFreq = (SineWave():freq(0.01) + 1) * 200 + 225;

local output = (( tone * env)) * 0.3;

synth:setOutputGen(output);