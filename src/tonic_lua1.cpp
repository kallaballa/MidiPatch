#include "tonic_lua.hpp"

using namespace Tonic;

void bindings1(kaguya::State& state) {
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
