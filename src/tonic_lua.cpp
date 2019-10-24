#include "tonic_lua.hpp"

using namespace Tonic;

void bindings(kaguya::State& state) {
	state["dBToLin"].setFunction(&dBToLin);
	state["linTodB"].setFunction(&linTodB);
	state["Synth"].setClass(
			kaguya::UserdataMetatable<Synth>()
			.setConstructors<Synth()>()
			.addFunction("setLimitOutput", &Synth::setLimitOutput)
			.addFunction("setOutputGen", &Synth::setOutputGen)
			.addOverloadedFunctions("addParameter",
					(ControlParameter (Synth::*)(string))&Synth::addParameter,
						(ControlParameter (Synth::*)(string, TonicFloat))&Synth::addParameter,
						(void (Synth::*)(ControlParameter))&Synth::addParameter));

//	auto fa = [](Generator a, TemplatedEffect<HPF24,Tonic_::HPF24_> b) { return stream(a,b);};
//	auto fb = [](Generator a, TemplatedEffect<LPF24,Tonic_::LPF24_> b) { return stream(a,b);};
//	auto fc = [](Generator a, TemplatedEffect<LPF12,Tonic_::LPF12_> b) { return stream(a,b);};
//
//	state["stream"] = kaguya::overload(fa, fb, fc);
//
	state["SampleTable"].setClass(
			kaguya::UserdataMetatable<SampleTable>()
			.setConstructors<SampleTable()>()
			.addFunction("channels", &SampleTable::channels)
			.addFunction("frames", &SampleTable::frames)
			.addFunction("size", &SampleTable::size)
			.addFunction("resize", &SampleTable::resize)
			.addFunction("resample", &SampleTable::resample)
			.addFunction("dataPointer", &SampleTable::dataPointer)
);


	state["Generator"].setClass(make_generator_operators(
			kaguya::UserdataMetatable<Generator>()
			.setConstructors<Generator()>()
			.addFunction("isStereoOutput", &Generator::isStereoOutput)
			.addFunction("tick", &Generator::tick)
					));



	state["Adder"].setClass(make_generator(state,
			kaguya::UserdataMetatable<Adder, TemplatedGenerator<Tonic_::Adder_>>()
			.setConstructors<Adder()>()
			.addFunction("input", &Adder::input)
			.addFunction("numInputs", &Adder::numInputs)));


	state["ControlAdder"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlAdder, TemplatedControlGenerator<Tonic_::ControlAdder_>>()
			.setConstructors<ControlAdder()>()
			.addFunction("input", &ControlAdder::input)
			.addFunction("numInputs", &ControlAdder::numInputs)));

	state["Subtractor"].setClass(make_generator(state,
			kaguya::UserdataMetatable<Subtractor, TemplatedGenerator<Tonic_::Subtractor_>>()
			.setConstructors<Subtractor()>()
			.addOverloadedFunctions("left",
					(Subtractor& (Subtractor::*)(Generator))&Subtractor::left,
					(Subtractor& (Subtractor::*)(float))&Subtractor::left,
					(Subtractor& (Subtractor::*)(ControlGenerator))&Subtractor::left)
			.addOverloadedFunctions("right",
					(Subtractor& (Subtractor::*)(Generator))&Subtractor::right,
					(Subtractor& (Subtractor::*)(float))&Subtractor::right,
					(Subtractor& (Subtractor::*)(ControlGenerator))&Subtractor::right)
			));

	state["ControlSubtractor"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlSubtractor, TemplatedControlGenerator<Tonic_::ControlSubtractor_>>()
			.setConstructors<ControlSubtractor()>()
			.addOverloadedFunctions("left",
					(ControlSubtractor& (ControlSubtractor::*)(float))&ControlSubtractor::left,
					(ControlSubtractor& (ControlSubtractor::*)(ControlGenerator))&ControlSubtractor::left)
			.addOverloadedFunctions("right",
					(ControlSubtractor& (ControlSubtractor::*)(float))&ControlSubtractor::right,
					(ControlSubtractor& (ControlSubtractor::*)(ControlGenerator))&ControlSubtractor::right)
			));

	state["Multiplier"].setClass(make_generator(state,
			kaguya::UserdataMetatable<Multiplier,TemplatedGenerator<Tonic_::Multiplier_>>()
			.setConstructors<Multiplier()>()
			.addFunction("input", &Multiplier::input)
			.addFunction("numInputs", &Multiplier::numInputs)));

	state["ControlMultiplier"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlMultiplier, TemplatedControlGenerator<Tonic_::ControlMultiplier_>>()
			.setConstructors<ControlMultiplier()>()
			.addFunction("input", &ControlMultiplier::input)
			.addFunction("numInputs", &ControlMultiplier::numInputs)));

	state["Divider"].setClass(make_generator(state,
				kaguya::UserdataMetatable<Divider, TemplatedGenerator<Tonic_::Divider_>>()
				.setConstructors<Divider()>()
				.addOverloadedFunctions("left",
						(Divider& (Divider::*)(Generator))&Divider::left,
						(Divider& (Divider::*)(float))&Divider::left,
						(Divider& (Divider::*)(ControlGenerator))&Divider::left)
				.addOverloadedFunctions("right",
						(Divider& (Divider::*)(Generator))&Divider::right,
						(Divider& (Divider::*)(float))&Divider::right,
						(Divider& (Divider::*)(ControlGenerator))&Divider::right)
			));

	state["ControlDivider"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlDivider, TemplatedControlGenerator<Tonic_::ControlDivider_>>()
			.setConstructors<ControlDivider()>()
			.addOverloadedFunctions("left",
					(ControlDivider& (ControlDivider::*)(float))&ControlDivider::left,
					(ControlDivider& (ControlDivider::*)(ControlGenerator))&ControlDivider::left)
			.addOverloadedFunctions("right",
					(ControlDivider& (ControlDivider::*)(float))&ControlDivider::right,
					(ControlDivider& (ControlDivider::*)(ControlGenerator))&ControlDivider::right)
			));

	state["FixedValue"].setClass(make_generator(state,
			kaguya::UserdataMetatable<FixedValue, TemplatedGenerator<Tonic_::FixedValue_>>()
			.setConstructors<FixedValue(), FixedValue(float)>()
			.addOverloadedFunctions("setValue",
			(FixedValue& (FixedValue::*)(float))&FixedValue::setValue,
			(FixedValue& (FixedValue::*)(ControlGenerator))&FixedValue::setValue)
	));

	state["RampedValue"].setClass(make_generator(state,
			kaguya::UserdataMetatable<RampedValue, TemplatedGenerator<Tonic_::RampedValue_>>()
			.setConstructors<RampedValue()>()
			.addFunction("isFinished", &RampedValue::isFinished)
			.addOverloadedFunctions("target",
					(RampedValue& (RampedValue::*)(float))&RampedValue::target,
					(RampedValue& (RampedValue::*)(ControlGenerator))&RampedValue::target)
			.addOverloadedFunctions("length",
					(RampedValue& (RampedValue::*)(float))&RampedValue::length,
					(RampedValue& (RampedValue::*)(ControlGenerator))&RampedValue::length)
			.addOverloadedFunctions("value",
					(RampedValue& (RampedValue::*)(float))&RampedValue::value,
					(RampedValue& (RampedValue::*)(ControlGenerator))&RampedValue::value)
			));

	state["SineWave"].setClass(make_generator(state,
			kaguya::UserdataMetatable<SineWave, TemplatedGenerator<Tonic_::TableLookupOsc_>>()
			.setConstructors<SineWave()>()
			.addOverloadedFunctions("freq",
					(SineWave& (SineWave::*)(Generator))&SineWave::freq,
					(SineWave& (SineWave::*)(float))&SineWave::freq,
					(SineWave& (SineWave::*)(ControlGenerator))&SineWave::freq)
				));

	state["RectWave"].setClass(make_generator(state,
			kaguya::UserdataMetatable<RectWave, TemplatedGenerator<Tonic_::RectWave_>>()
			.setConstructors<RectWave()>()
			.addOverloadedFunctions("freq",
					(RectWave& (RectWave::*)(Generator))&RectWave::freq,
					(RectWave& (RectWave::*)(float))&RectWave::freq,
					(RectWave& (RectWave::*)(ControlGenerator))&RectWave::freq)
			.addOverloadedFunctions("pwm",
					(RectWave& (RectWave::*)(Generator))&RectWave::pwm,
					(RectWave& (RectWave::*)(float))&RectWave::pwm,
					(RectWave& (RectWave::*)(ControlGenerator))&RectWave::pwm)
				));

	state["RectWaveBL"].setClass(make_generator(state,
			kaguya::UserdataMetatable<RectWaveBL, TemplatedGenerator<Tonic_::RectWaveBL_>>()
			.setConstructors<RectWaveBL()>()
			.addOverloadedFunctions("freq",
					(RectWaveBL& (RectWaveBL::*)(Generator))&RectWaveBL::freq,
					(RectWaveBL& (RectWaveBL::*)(float))&RectWaveBL::freq,
					(RectWaveBL& (RectWaveBL::*)(ControlGenerator))&RectWaveBL::freq)
			.addOverloadedFunctions("pwm",
					(RectWaveBL& (RectWaveBL::*)(Generator))&RectWaveBL::pwm,
					(RectWaveBL& (RectWaveBL::*)(float))&RectWaveBL::pwm,
					(RectWaveBL& (RectWaveBL::*)(ControlGenerator))&RectWaveBL::pwm)
	));

	state["SquareWave"].setClass(make_generator(state,
			kaguya::UserdataMetatable<SquareWave, TemplatedGenerator<Tonic_::RectWave_>>()
			.setConstructors<SquareWave()>()
			.addOverloadedFunctions("freq",
					(SquareWave& (SquareWave::*)(Generator))&SquareWave::freq,
					(SquareWave& (SquareWave::*)(float))&SquareWave::freq,
					(SquareWave& (SquareWave::*)(ControlGenerator))&SquareWave::freq)
	));

	state["SquareWaveBL"].setClass(make_generator(state,
			kaguya::UserdataMetatable<SquareWaveBL, TemplatedGenerator<Tonic_::RectWaveBL_>>()
			.setConstructors<SquareWaveBL()>()
			.addOverloadedFunctions("freq",
					(SquareWaveBL& (SquareWaveBL::*)(Generator))&SquareWaveBL::freq,
					(SquareWaveBL& (SquareWaveBL::*)(float))&SquareWaveBL::freq,
					(SquareWaveBL& (SquareWaveBL::*)(ControlGenerator))&SquareWaveBL::freq)
	));
	state["Noise"].setClass(make_generator(state,
				kaguya::UserdataMetatable<Noise, TemplatedGenerator<Tonic_::Noise_>>()
				.setConstructors<Noise()>()
				));

	state["HPF24"].setClass(make_generator(state,
			kaguya::UserdataMetatable<HPF24, TemplatedGenerator<Tonic_::HPF24_>>()
			.setConstructors<HPF24()>()
			.addFunction("normalizesGain", (HPF24& (HPF24::*)(bool))&HPF24::normalizesGain)
			.addFunction("input", (HPF24& (HPF24::*)(Generator))&HPF24::input)
			.addOverloadedFunctions("cutoff",
					(HPF24& (HPF24::*)(Generator))&HPF24::cutoff,
					(HPF24& (HPF24::*)(float))&HPF24::cutoff)
					.addOverloadedFunctions("Q",
							(HPF24& (HPF24::*)(Generator))&HPF24::Q,
							(HPF24& (HPF24::*)(float))&HPF24::Q)
));

	state["LPF12"].setClass(make_generator(state,
			kaguya::UserdataMetatable<LPF12, TemplatedGenerator<Tonic_::LPF12_>>()
			.setConstructors<LPF12()>()
			.addFunction("normalizesGain", (LPF12& (LPF12::*)(bool))&LPF12::normalizesGain)
			.addFunction("input", (LPF12& (LPF12::*)(Generator))&LPF12::input)
			.addOverloadedFunctions("cutoff",
					(LPF12& (LPF12::*)(Generator))&LPF12::cutoff,
					(LPF12& (LPF12::*)(float))&LPF12::cutoff)
					.addOverloadedFunctions("Q",
							(LPF12& (LPF12::*)(Generator))&LPF12::Q,
							(LPF12& (LPF12::*)(float))&LPF12::Q)
							));

	state["LPF24"].setClass(make_generator(state,
			kaguya::UserdataMetatable<LPF24, TemplatedGenerator<Tonic_::LPF24_>>()
			.setConstructors<LPF24()>()
			.addFunction("normalizesGain", (LPF24& (LPF24::*)(bool))&LPF24::normalizesGain)
			.addFunction("input", (LPF24& (LPF24::*)(Generator))&LPF24::input)
			.addOverloadedFunctions("cutoff",
					(LPF24& (LPF24::*)(Generator))&LPF24::cutoff,
					(LPF24& (LPF24::*)(float))&LPF24::cutoff)
					.addOverloadedFunctions("Q",
							(LPF24& (LPF24::*)(Generator))&LPF24::Q,
							(LPF24& (LPF24::*)(float))&LPF24::Q)
						));

	state["ControlGenerator"].setClass(make_control_operators(
			kaguya::UserdataMetatable<ControlGenerator>()
			.setConstructors<ControlGenerator()>()
			.addFunction("smoothed", &ControlGenerator::smoothed)
			.addFunction("tick", &ControlGenerator::tick))
		);

	state["ControlParameter"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlParameter, TemplatedControlGenerator<Tonic_::ControlParameter_>>()
			.setConstructors<ControlParameter()>()
			.addFunction("getName", &ControlParameter::getName)
			.addFunction("name", &ControlParameter::name)
			.addFunction("getDisplayName", &ControlParameter::getDisplayName)
			.addFunction("displayName", &ControlParameter::displayName)
			.addFunction("getValue", &ControlParameter::getValue)
			.addFunction("value", &ControlParameter::value)
			.addFunction("getMin", &ControlParameter::getMin)
			.addFunction("min", &ControlParameter::min)
			.addFunction("getMax", &ControlParameter::getMax)
			.addFunction("max", &ControlParameter::max)
			.addFunction("getParameterType", &ControlParameter::getParameterType)
			.addFunction("parameterType", &ControlParameter::parameterType)
			.addFunction("getIsLogarithmic", &ControlParameter::getIsLogarithmic)
			.addFunction("logarithmic", &ControlParameter::logarithmic)
			.addFunction("getNormalizedValue", &ControlParameter::getNormalizedValue)
			.addFunction("setNormalizedValue", &ControlParameter::setNormalizedValue)
));

	state["ControlRandom"].setClass(make_control(state,
				kaguya::UserdataMetatable<ControlRandom, TemplatedControlGenerator<Tonic_::ControlRandom_>>()
				.setConstructors<ControlRandom()>()
				.addOverloadedFunctions("max",
						(ControlRandom& (ControlRandom::*)(ControlGenerator))&ControlRandom::max,
						(ControlRandom& (ControlRandom::*)(float))&ControlRandom::max)
				.addOverloadedFunctions("min",
						(ControlRandom& (ControlRandom::*)(ControlGenerator))&ControlRandom::min,
						(ControlRandom& (ControlRandom::*)(float))&ControlRandom::min)
				.addOverloadedFunctions("trigger",
						(ControlRandom& (ControlRandom::*)(ControlGenerator))&ControlRandom::trigger,
						(ControlRandom& (ControlRandom::*)(float))&ControlRandom::trigger)
	));


	state["ControlMetro"].setClass(make_control(state,
				kaguya::UserdataMetatable<ControlMetro,TemplatedControlGenerator<Tonic_::ControlMetro_>>()
				.setConstructors<ControlMetro(), ControlMetro(float)>()
				.addOverloadedFunctions("bpm",
						(ControlMetro& (ControlMetro::*)(ControlGenerator))&ControlMetro::bpm,
						(ControlMetro& (ControlMetro::*)(float))&ControlMetro::bpm)
	));


	state["ControlPulse"].setClass(make_control(state,
				kaguya::UserdataMetatable<ControlPulse,TemplatedControlGenerator<Tonic_::ControlPulse_>>()
				.setConstructors<ControlPulse(),ControlPulse(float)>()
				.addOverloadedFunctions("trigger",
						(ControlPulse& (ControlPulse::*)(ControlGenerator))&ControlPulse::trigger,
						(ControlPulse& (ControlPulse::*)(float))&ControlPulse::trigger)
					.addOverloadedFunctions("length",
							(ControlPulse& (ControlPulse::*)(ControlGenerator))&ControlPulse::length,
							(ControlPulse& (ControlPulse::*)(float))&ControlPulse::length)));

	state["ControlSnapToScale"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlSnapToScale,TemplatedControlGenerator<Tonic_::ControlSnapToScale_>>()
			.setConstructors<ControlSnapToScale()>()
			.addFunction("setScale", &ControlSnapToScale::setScale)
			.addOverloadedFunctions("input",
				(ControlSnapToScale (ControlSnapToScale::*)(float))&ControlSnapToScale::input,
				(ControlSnapToScale (ControlSnapToScale::*)(ControlGenerator))&ControlSnapToScale::input)));

	state["ControlMidiToFreq"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlMidiToFreq,TemplatedControlGenerator<Tonic_::ControlMidiToFreq_>>()
			.setConstructors<ControlMidiToFreq()>()
			.addOverloadedFunctions("input",
				(ControlMidiToFreq (ControlMidiToFreq::*)(float))&ControlMidiToFreq::input,
				(ControlMidiToFreq (ControlMidiToFreq::*)(ControlGenerator))&ControlMidiToFreq::input)));

	state["ADSR"].setClass(make_generator(state,
			kaguya::UserdataMetatable<ADSR, TemplatedGenerator<Tonic_::ADSR_>>()
			.setConstructors<ADSR()>()
					.addOverloadedFunctions("trigger",
					(ADSR& (ADSR::*)(float))&ADSR::trigger,
					(ADSR& (ADSR::*)(ControlGenerator))&ADSR::trigger)
					.addOverloadedFunctions("attack",
					(ADSR& (ADSR::*)(float))&ADSR::attack,
					(ADSR& (ADSR::*)(ControlGenerator))&ADSR::attack)
					.addOverloadedFunctions("decay",
					(ADSR& (ADSR::*)(float))&ADSR::decay,
					(ADSR& (ADSR::*)(ControlGenerator))&ADSR::decay)
					.addOverloadedFunctions("sustain",
					(ADSR& (ADSR::*)(float))&ADSR::sustain,
					(ADSR& (ADSR::*)(ControlGenerator))&ADSR::sustain)
					.addOverloadedFunctions("release",
					(ADSR& (ADSR::*)(float))&ADSR::release,
					(ADSR& (ADSR::*)(ControlGenerator))&ADSR::release)
					.addOverloadedFunctions("exponential",
					(ADSR& (ADSR::*)(float))&ADSR::exponential,
					(ADSR& (ADSR::*)(ControlGenerator))&ADSR::exponential)
					.addOverloadedFunctions("doesSustain",
					(ADSR& (ADSR::*)(float))&ADSR::doesSustain,
					(ADSR& (ADSR::*)(ControlGenerator))&ADSR::doesSustain)
					.addOverloadedFunctions("legato",
					(ADSR& (ADSR::*)(float))&ADSR::legato,
					(ADSR& (ADSR::*)(ControlGenerator))&ADSR::legato)));

	state["Compressor"].setClass(make_generator(state,
			kaguya::UserdataMetatable<Compressor,TemplatedGenerator<Tonic_::Compressor_>>()
			.setConstructors<Compressor()>()
			.addFunction("input", (Compressor& (Compressor::*)(Generator))&Compressor::input)
			.addFunction("audioInput", &Compressor::audioInput)
			.addFunction("sidechainInput", &Compressor::sidechainInput)
			.addFunction("setIsStereo", &Compressor::setIsStereo)
					.addOverloadedFunctions("attack",
							(Compressor& (Compressor::*)(float))&Compressor::attack,
							(Compressor& (Compressor::*)(ControlGenerator))&Compressor::attack)
					.addOverloadedFunctions("release",
							(Compressor& (Compressor::*)(float))&Compressor::release,
							(Compressor& (Compressor::*)(ControlGenerator))&Compressor::release)
					.addOverloadedFunctions("threshold",
							(Compressor& (Compressor::*)(float))&Compressor::threshold,
							(Compressor& (Compressor::*)(ControlGenerator))&Compressor::threshold)
					.addOverloadedFunctions("ratio",
							(Compressor& (Compressor::*)(float))&Compressor::ratio,
							(Compressor& (Compressor::*)(ControlGenerator))&Compressor::ratio)
					.addOverloadedFunctions("lookahead",
							(Compressor& (Compressor::*)(float))&Compressor::lookahead,
							(Compressor& (Compressor::*)(ControlGenerator))&Compressor::lookahead)
					.addOverloadedFunctions("makeupGain",
							(Compressor& (Compressor::*)(float))&Compressor::makeupGain,
							(Compressor& (Compressor::*)(ControlGenerator))&Compressor::makeupGain)));
}
