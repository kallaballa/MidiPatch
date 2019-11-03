#pragma once

#include "Tonic.h"

using namespace Tonic;

template<typename VoiceAllocator>
class PolySynthWithAllocator: public Synth {
public:
	PolySynthWithAllocator() :
			Synth() {
		setOutputGen(mixer);
	}

	virtual ~PolySynthWithAllocator() {

	}

	void addVoice(Synth synth) {
		allocator.addVoice(synth);
		mixer.addInput(synth);
	}

	std::vector<typename VoiceAllocator::PolyVoice>& getVoices() {
		return allocator.getVoices();
	}

	typedef Synth (VoiceCreateFn)();
	void addVoices(VoiceCreateFn createFn, int count) {
		for (int i = 0; i < count; i++)
			addVoice(createFn());
	}

	void noteOn(int note, int velocity) {
		allocator.noteOn(note, velocity);
	}

	void noteOff(int note) {
		allocator.noteOff(note);
	}

protected:
	Mixer mixer;
	VoiceAllocator allocator;
};

class BasicPolyphonicAllocator {
public:
	class PolyVoice {
	public:
		int currentNote;
		Synth synth;
	};

	virtual ~BasicPolyphonicAllocator() {

	}
	void addVoice(Synth synth);
	void noteOn(int noteNumber, int velocity);
	void noteOff(int noteNumber);
	vector<PolyVoice>& getVoices();
protected:
	virtual int getNextVoice(int note);
	vector<PolyVoice> voiceData;
	list<int> inactiveVoiceQueue;
	list<int> activeVoiceQueue;
};

class OldestNoteStealingPolyphonicAllocator: public BasicPolyphonicAllocator {
public:
	virtual ~OldestNoteStealingPolyphonicAllocator() {

	}
protected:
	virtual int getNextVoice(int note);
};

class LowestNoteStealingPolyphonicAllocator: public BasicPolyphonicAllocator {
public:
	virtual ~LowestNoteStealingPolyphonicAllocator() {

	}
protected:
	virtual int getNextVoice(int note);
};

typedef PolySynthWithAllocator<LowestNoteStealingPolyphonicAllocator> PolySynth;

