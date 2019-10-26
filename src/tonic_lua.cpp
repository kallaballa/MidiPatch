#include "tonic_lua.hpp"

using namespace Tonic;

void bindings1(kaguya::State& state) {
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

	state["TriangleWave"].setClass(make_generator(state,
			kaguya::UserdataMetatable<TriangleWave, TemplatedGenerator<Tonic_::AngularWave_>>()
			.setConstructors<TriangleWave()>()
			.addOverloadedFunctions("freq",
					(TriangleWave& (TriangleWave::*)(Generator))&TriangleWave::freq,
					(TriangleWave& (TriangleWave::*)(float))&TriangleWave::freq,
					(TriangleWave& (TriangleWave::*)(ControlGenerator))&TriangleWave::freq)
			.addOverloadedFunctions("slope",
					(TriangleWave& (TriangleWave::*)(Generator))&TriangleWave::slope,
					(TriangleWave& (TriangleWave::*)(float))&TriangleWave::slope,
					(TriangleWave& (TriangleWave::*)(ControlGenerator))&TriangleWave::slope)
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

	state["StereoDelay"].setClass(make_wet_dry(state,
			kaguya::UserdataMetatable<StereoDelay,TemplatedWetDryEffect<StereoDelay,Tonic_::StereoDelay_>>()
			.setConstructors<StereoDelay(float,float,float,float)>()
			.addOverloadedFunctions("feedback",
					(StereoDelay& (StereoDelay::*)(Generator))&StereoDelay::feedback,
					(StereoDelay& (StereoDelay::*)(float))&StereoDelay::feedback,
					(StereoDelay& (StereoDelay::*)(ControlGenerator))&StereoDelay::feedback)
			.addOverloadedFunctions("delayTimeLeft",
					(StereoDelay& (StereoDelay::*)(Generator))&StereoDelay::delayTimeLeft,
					(StereoDelay& (StereoDelay::*)(float))&StereoDelay::delayTimeLeft,
					(StereoDelay& (StereoDelay::*)(ControlGenerator))&StereoDelay::delayTimeLeft)
			.addOverloadedFunctions("delayTimeRight",
					(StereoDelay& (StereoDelay::*)(Generator))&StereoDelay::delayTimeRight,
					(StereoDelay& (StereoDelay::*)(float))&StereoDelay::delayTimeRight,
					(StereoDelay& (StereoDelay::*)(ControlGenerator))&StereoDelay::delayTimeRight)
					));
}
