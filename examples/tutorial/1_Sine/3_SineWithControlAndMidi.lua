--[[
This examples shows how to generate a sine wave driven by MIDI 
events and control parameters. Note that in this example the
volume parameter is in Db.
]]--

--[[
The MIDI note parameters send by the engine. Note that they
all start with an underscore, which means they are private 
and are not included in the rack.
]]--
local noteNum = synth:addParameter("_polyNote", 0.0)
local gate = synth:addParameter("_polyGate", 0.0)
local noteVelocity = synth:addParameter("_polyVelocity", 0.0)
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0)

--[[
The global volume parameter. Scales the signal at the very end
]]--
local volume = synth:addParameter("Global.Volume", -10):displayName("Volume (Db)"):min(-30):max(30):logarithmic(true)

--[[
A ControlGenerator that takes MIDI note numbers as input and
outputs the frequency in Hz.
]]--
local freq = ControlMidiToFreq():input(noteNum)

--[[
The sine wave oscillator generating the actual tone at the
note frequency
]]--
local tone = SineWave():freq(freq);

--[[
This parameters is driven by the note velocity and leads to 
a louder tone with a higher velocity.
]]--
local velocityMod = FixedValue(0.02) + noteVelocity * 0.005;

--[[
we need the envelope for two reasons:
1. to prevent clicking on noteOn and noteOff (hence the slight
   attack and release)
2. So we can use it as a gate. Without it the oscillator would 
   be generating a continous signal
]]--
local envelope = ADSR()
        :attack(0.01)
        :decay(0)
        :sustain(1)
        :release(0.01)
        :doesSustain(1)
        :trigger(gate)

--[[
Attenuate the "tone" (sine wave) by the "velocityMod" (higher 
velocity -> louder tone). Then apply the "envelope" (which in
our case does nearly nothing) and scale the signal by
"volume". The "volume" parameter is in Db, hence the
conversion to linear.
]]--
local signal = tone * velocityMod * envelope * ControlDbToLinear():input(volume);

--[[
Assign the generator "signal" to the synth.
]]--
synth:setOutputGen(signal);
