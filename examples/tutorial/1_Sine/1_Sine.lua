--[[
This examples shows how to generate a continous 440Hz sine 
wave signal without listening for MIDI events and without 
any control parameters.
]]--


--[[
The sine wave oscillator generating the actual tone at 
440Hz
]]--
local tone = SineWave():freq(440);

--[[
Assign the generator to the synth.
]]--
synth:setOutputGen(tone);
