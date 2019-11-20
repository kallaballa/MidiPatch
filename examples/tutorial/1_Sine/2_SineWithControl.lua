--[[
This examples shows how to generate a continous sine wave 
with a control parameters.
]]--

--[[
The volume parameter. Scales the signal at the very end.
Control parameters automatically show up in the rack
]]--
local volume = synth:addParameter("Global.Volume", 10).displayName("Volume (%)"):min(0):max(100);

--[[
The frequency parameter which controls the pitch of the 
tone
]]--
local freq = synth:addParameter("Global.Frequency", 440).displayName("Frequency (Hz)"):min(10):max(10000);


--[[
The sine wave oscillator generating the actual tone at the
note frequency
]]--
local tone = SineWave():freq(freq);

--[[
Scale the sine wave signal by volume
]]--
local signal = tone * (volume / 100)

--[[
Assign the generator "signal" to the synth.
]]--
synth:setOutputGen(signal);
