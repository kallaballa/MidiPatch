#ifndef TONIC_LUA_HPP_
#define TONIC_LUA_HPP_

#include "kaguya/kaguya.hpp"
#include "Tonic.h"

using namespace Tonic;

template<class Tclass, class Tbase>
kaguya::UserdataMetatable<Tclass, Tbase>& make_generator_operators(kaguya::UserdataMetatable<Tclass, Tbase>& tbl) {
	tbl
			.addOverloadedFunctions("__add",
			(Adder (Tclass::*)(float))&Tclass::operator+,
			(Adder (Tclass::*)(ControlGenerator))&Tclass::operator+,
			(Adder (Tclass::*)(Generator))&Tclass::operator+)
			.addOverloadedFunctions("__sub",
			(Subtractor (Tclass::*)(float))&Tclass::operator-,
			(Subtractor (Tclass::*)(ControlGenerator))&Tclass::operator-,
			(Subtractor (Tclass::*)(Generator))&Tclass::operator-)
			.addOverloadedFunctions("__mul",
			(Multiplier (Tclass::*)(float))&Tclass::operator*,
			(Multiplier (Tclass::*)(ControlGenerator))&Tclass::operator*,
			(Multiplier (Tclass::*)(Generator))&Tclass::operator*)
			.addOverloadedFunctions("__div",
			(Divider (Tclass::*)(float))&Tclass::operator/,
			(Divider (Tclass::*)(ControlGenerator))&Tclass::operator/,
			(Divider (Tclass::*)(Generator))&Tclass::operator/);
			return tbl;
}

template<class Tclass, class Tbase>
kaguya::UserdataMetatable<Tclass, Tbase>& make_control_operators(kaguya::UserdataMetatable<Tclass, Tbase>& tbl) {
	tbl.addOverloadedFunctions("__add",
			(ControlAdder (Tclass::*)(float))&Tclass::operator+,
			(ControlAdder (Tclass::*)(ControlGenerator))&Tclass::operator+)
			.addOverloadedFunctions("__sub",
			(ControlSubtractor (Tclass::*)(float))&Tclass::operator-,
			(ControlSubtractor (Tclass::*)(ControlGenerator))&Tclass::operator-)
			.addOverloadedFunctions("__mul",
			(ControlMultiplier (Tclass::*)(float))&Tclass::operator*,
			(ControlMultiplier (Tclass::*)(ControlGenerator))&Tclass::operator*)
			.addOverloadedFunctions("__div",
			(ControlDivider (Tclass::*)(float))&Tclass::operator/,
			(ControlDivider (Tclass::*)(ControlGenerator))&Tclass::operator/);
			return tbl;
}


template<class Tclass, class Tclass_> kaguya::UserdataMetatable<Tclass,TemplatedGenerator<Tclass_>> make_generator(kaguya::State& state, kaguya::UserdataMetatable<Tclass,TemplatedGenerator<Tclass_>>& tbl) {
	typedef TemplatedGenerator<Tclass_> TG_t;
	state[string("TemplatedGenerator") + typeid(Tclass).name()].setClass(
				kaguya::UserdataMetatable<TG_t,Generator>());
	return make_generator_operators(tbl);
}

template<class Tclass, class Tclass_> kaguya::UserdataMetatable<Tclass,TemplatedControlGenerator<Tclass_>> make_control(kaguya::State& state, kaguya::UserdataMetatable<Tclass,TemplatedControlGenerator<Tclass_>>& tbl) {
	typedef TemplatedControlGenerator<Tclass_> TG_t;
	state[string("TemplatedControlGenerator") + typeid(Tclass).name()].setClass(
				kaguya::UserdataMetatable<TG_t,ControlGenerator>());
	return make_control_operators(tbl);
}

void bindings(kaguya::State& state);


#endif /* TONIC_LUA_HPP_ */
