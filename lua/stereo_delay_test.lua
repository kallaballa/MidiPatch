local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);

local voiceFreq = ControlMidiToFreq():input(noteNum)

    local tone = RectWave():freq(
        voiceFreq)
-- * synth:addParameter("frequencyRandomAmount") + 100
  --      + synth:addParameter("freq")
    --  ) * SineWave():freq(50);
    
    local env = ADSR()
      :attack(0.01)
      :decay( synth:addParameter("decay",0) )
      :sustain(0)
      :release(0)
      :doesSustain(0)
      :trigger(gate);
    
    local delay = StereoDelay(3.0,3.0, -1, -1)
    :delayTimeLeft( FixedValue(0.5) + SineWave():freq(0.2) * 0.01)
    :delayTimeRight( FixedValue(0.55) + SineWave():freq(0.23) * 0.01)
    :feedback(0.3)
    :dryLevel(0.8)
    :wetLevel(0.2);
    
    local filterFreq = (SineWave():freq(0.01) + 1) * 200 + 225;
    
    local filter = LPF24():Q(2):cutoff( filterFreq );

    local outputGen = filter:input(delay:input( tone * env )) * (FixedValue(0.02) + noteVelocity * 0.005) * 0.3;
synth:setOutputGen(outputGen);
