--[[
This example demonstrates how beats and melodies can be created generatively    
--]]
local scale = {0,2,3,7,10};
local speed = synth:addParameter("Global.speed", 0.85);
local switchIt = ControlMetro():bpm(speed * 30);

local metro = ControlMetro():bpm(
    ControlStepper()
      :bidirectional(1)
      :start(550)
      :end_(650)
      :step(100)
      :trigger(switchIt)
    * speed
  );


local env = ADSR(0.01, 0.5, 0, 0.1)
	:trigger(metro)
	:doesSustain(0)
	:legato(1)
	:decay(
		ControlStepper()
		:start(0.05)
		:end_(0.5)
		:step(0.01)
		:trigger(metro)
	);

local stepperStart = synth:addParameter("Global.stepperStart") * 30 + 43;

local stepper1 = ControlStepper()
	:bidirectional(1)
	:start( stepperStart )
	:end_(stepperStart + (synth:addParameter("Global.stepperSpread") * 50))
	:step(
		ControlStepper()
		:bidirectional(1)
		:start(4)
		:end_(
    		ControlRandom()
    		:min(9)
    		:max(15)
    		:trigger(
      			ControlMetro()
				:bpm(11)
    		)
	)
	:step(1)
	:trigger(metro * 1)
)
:trigger(metro);

local scaleSnapper1 =  ControlSnapToScale()
	:setScale(scale)
	:input( stepper1 );

local delayMix = ControlRandom()
	:min(0)
	:max(0.1)
	:trigger(switchIt)
	:smoothed(0.05);

local verb = Reverb()
	:inputHPFCutoff(200)
	:decayTime(0.8)
	:roomSize(0.1)
	:density(0.8)
	:wetLevel(dBToLin(-30));

local tone = ((
	SineWave()
	:freq(
    	ControlMidiToFreq()
		:input(scaleSnapper1 + -12)
		:smoothed(0.05)
		:length(
      		ControlRandom():min(0):max(0.03):trigger(switchIt)
    	)
	) +
	SineWave()
	:freq(
    	ControlMidiToFreq():input(scaleSnapper1 + 12)
	) * 0.1
) * 0.3 * env);

local stereoDelay = StereoDelay(0.3,0.33,0,0)
	:feedback(0.3)
	:dryLevel(FixedValue(1.0) - delayMix)
	:wetLevel(delayMix);

local outputGen = verb:input(stereoDelay:input(tone))

synth:setOutputGen(outputGen);
