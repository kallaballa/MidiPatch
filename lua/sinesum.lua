local NUM_SINES=3

local noteNum = synth:addParameter("_polyNote", 0.0);
local gate = synth:addParameter("_polyGate", 0.0);
local noteVelocity = synth:addParameter("_polyVelocity", 0.0);
local voiceNumber = synth:addParameter("_polyVoiceNumber", 0.0);

local voiceFreq = ControlMidiToFreq():input(noteNum)
 local env = ADSR()
        :attack(0.01)
        :decay(0)
        :sustain(1)
        :release(0)
        :doesSustain(1)
        :trigger(gate);



    local outputAdder = Adder();
    
    for s=0,NUM_SINES do
      local pitchGen = ((voiceFreq * 220 + 220) * math.pow(2, (s - (NUM_SINES/2)) * 5.0 / 12.0));
	print(pitchGen)
      outputAdder:input(SineWave():freq( pitchGen:smoothed(0.05) ));
    end
    
    local outputGen = outputAdder * ((1.0/NUM_SINES) * 0.5);
    
    synth:setOutputGen((outputGen * env) * (FixedValue(0.02) + noteVelocity * 0.005));

