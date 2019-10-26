#include "tonic_lua.hpp"

using namespace Tonic;

void bindings2(kaguya::State& state) {
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


	state["ControlGenerator"].setClass(make_control_operators(
			kaguya::UserdataMetatable<ControlGenerator>()
			.setConstructors<ControlGenerator()>()
			.addFunction("smoothed", &ControlGenerator::smoothed)
			.addFunction("tick", &ControlGenerator::tick))
		);

	state["ControlAdder"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlAdder, TemplatedControlGenerator<Tonic_::ControlAdder_>>()
			.setConstructors<ControlAdder()>()
			.addFunction("input", &ControlAdder::input)
			.addFunction("numInputs", &ControlAdder::numInputs)));

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

	state["ControlMultiplier"].setClass(make_control(state,
			kaguya::UserdataMetatable<ControlMultiplier, TemplatedControlGenerator<Tonic_::ControlMultiplier_>>()
			.setConstructors<ControlMultiplier()>()
			.addFunction("input", &ControlMultiplier::input)
			.addFunction("numInputs", &ControlMultiplier::numInputs)));

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
}
