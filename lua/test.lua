local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);
local gain = synth:addParameter("Global.Gain", 1.0);
local sine = synth:addParameter("Global.Sine", 1.0);
local square = synth:addParameter("Global.Square", 1.0);
local triangle = synth:addParameter("Global.Triangle", 1.0);
local lpCutoff = synth:addParameter("Low Pass.Frequency", 1.0);
local lpQ = synth:addParameter("Low Pass.Resonance", 1.0);
local hpCutoff = synth:addParameter("High Pass.Frequency", 1.0);
local hpQ = synth:addParameter("High Pass.Resonance", 1.0);


local voiceFreq = ControlMidiToFreq():input(noteNum)
local tone0 = SineWave():freq(voiceFreq)
local tone1 = SquareWave():freq(voiceFreq)
local tone2 = TriangleWave():freq(voiceFreq)
local mix = ((tone0 * sine) + (tone1 * square) + (tone2 * triangle)) / 3.0 * gain;

local lpf = LPF24():cutoff(FixedValue(10000) * lpCutoff):Q(FixedValue(20) * lpQ)
local hpf = HPF24():cutoff(FixedValue(10000) * hpCutoff):Q(FixedValue(20) * hpQ)


local env = ADSR()
:attack(synth:addParameter("Envelope.Attack",0.1))
:decay(synth:addParameter("Envelope.Decay", 0 ))
:sustain(synth:addParameter("Envelope.Sustain",1))
:release(synth:addParameter("Envelope.Release",0.1))
:doesSustain(1)
:trigger(gate)

synth:setOutputGen((hpf:input(lpf:input(mix)) * env)
  * (FixedValue(0.02) + noteVelocity * 0.005));



