--[[
This example adds a number of oscillators and filters with each 
oscillator having a slightly different pitch that is slightly 
randomized. Addionally it adds a compressor to dampen the effects
of resonance.
]]--

local NUM_OSCILLATORS=10

local noteNum = synth:addParameter("_polyNote", 0.0)
local gate = synth:addParameter("_polyGate", 0.0)
local noteVelocity = synth:addParameter("_polyVelocity", 0.0)
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0)

local volume = synth:addParameter("Global.Volume", 0.3)
local attack = synth:addParameter("Global.Attack",0.1)
local decay = synth:addParameter("Global.Decay", 0 )
local sustain = synth:addParameter("Global.Sustain",1)
local release = synth:addParameter("Global.Release",0.1)
local shiftAmount = synth:addParameter("Global.PitchShiftAmount", 0)

local lpCutoff = synth:addParameter("Filter.LP_Freq", 1.0)
local lpQ = synth:addParameter("Filter.LP_Q", 0.0)
local hpCutoff = synth:addParameter("Filter.HP_Freq", 0.0)
local hpQ = synth:addParameter("Filter.HP_Q", 0.0)

local threshold = synth:addParameter("Compressor.threshold", -12):displayName("Threshold (dbFS)"):min(-60):max(0);
local ratio = synth:addParameter("Compressor.ratio", 2.0):displayName("Ratio"):min(1.0):max(64):logarithmic(1);
local attack = synth:addParameter("Compressor.attackTime", 0.001):displayName("Attack Time (s)"):min(0.001):max(0.1):logarithmic(true);
local release = synth:addParameter("Compressor.releaseTime", 0.05):displayName("Release Time (s)"):min(0.01):max(0.08):logarithmic(true);
local gain = synth:addParameter("Compressor.gain", 0):displayName( "Makeup Gain (dbFS)"):min(0):max(36);
local bypass = synth:addParameter("Compressor.bypass",0):parameterType(1);

local voiceFreq = ControlMidiToFreq():input(noteNum)
local randomShift = ControlRandom():min(-shiftAmount):max(shiftAmount);

function makeOSC(index, total, voiceFreq)
  local name = "Oscillator" .. (index - 1)
  local pitch = 1.0 - (0.01 * (index / total))
  local gain = synth:addParameter(name .. ".Gain", 1.0)
  local sine = synth:addParameter(name .. ".Sine", 1.0)
  local sineP = synth:addParameter(name .. ".SinePitch", pitch)
  local square = synth:addParameter(name .. ".Square", 0.25)
  local squareP = synth:addParameter(name .. ".SquarePitch", pitch)
  local triangle = synth:addParameter(name .. ".Triangle", 0.5)
  local triangleP = synth:addParameter(name .. ".TrianglePitch", pitch)
  return  (
            (SineWave():freq(voiceFreq * (sineP + randomShift)) * sine) +
            (SquareWave():freq(voiceFreq * (squareP + randomShift)) * square) +
            (TriangleWave():freq(voiceFreq * (triangleP + randomShift)) * triangle)
          ) * 0.33 * gain
end

local mix = Adder();
for i=1,NUM_OSCILLATORS do
	mix = mix + makeOSC(i,10,voiceFreq)     
end

local lpf = LPF24()
	:cutoff(FixedValue(10000) * lpCutoff)
	:Q(FixedValue(20) * lpQ)

local hpf = HPF24()
	:cutoff(FixedValue(10000) * hpCutoff)
	:Q(FixedValue(20) * hpQ)

local compressor = Compressor()
    :attack(attack)
    :release(0.06)
    :threshold( ControlDbToLinear():input(threshold) )
    :ratio( ratio )
    :lookahead(0.001)
    :makeupGain(ControlDbToLinear():input(gain))
    :bypass(bypass);

local env = ADSR()
	:attack(attack)
	:decay(decay)
	:sustain(sustain)
	:release(release)
	:doesSustain(1)
	:trigger(gate)

local velocity = FixedValue(0.02) + noteVelocity * 0.005

synth:setOutputGen(compressor:input(hpf:input(lpf:input(mix))) * velocity * env * volume);
