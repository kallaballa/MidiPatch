//
//  PolySynth.cpp
//  PolyMIDIDemo
//
//  Created by Michael Dewberry on 6/7/13.
//
//

#include "PolySynth.hpp"

void BasicPolyphonicAllocator::addVoice(Synth synth)
{
    PolyVoice v;
    v.synth = synth;
    v.currentNote = 0;

    inactiveVoiceQueue.push_back(voiceData.size());
    voiceData.push_back(v);
}

void BasicPolyphonicAllocator::noteOn(int note, int velocity)
{
    int voiceNumber = getNextVoice(note);

    if (voiceNumber < 0)
        return; // no voice available

    cout << ">> " << "Starting note " << note << " on voice " << voiceNumber << "\n";

    PolyVoice& voice = voiceData[voiceNumber];

    voice.synth.setParameter("_polyNote", note);
    voice.synth.setParameter("_polyGate", 1.0);
    voice.synth.setParameter("_polyVelocity", velocity);
    voice.synth.setParameter("_polyVoiceNumber", voiceNumber);

    voice.currentNote = note;

    activeVoiceQueue.remove(voiceNumber);
    activeVoiceQueue.push_back(voiceNumber);
    inactiveVoiceQueue.remove(voiceNumber);
}

void BasicPolyphonicAllocator::noteOff(int note)
{
    // clear the oldest active voice with this note number
    for (int voiceNumber : activeVoiceQueue)
    {
        PolyVoice& voice = voiceData[voiceNumber];
        if (voice.currentNote == note)
        {
            cout << ">> " << "Stopping note " << note << " on voice " << voiceNumber << "\n";

            voice.synth.setParameter("_polyGate", 0.0);

            activeVoiceQueue.remove(voiceNumber);
            inactiveVoiceQueue.remove(voiceNumber);
            inactiveVoiceQueue.push_back(voiceNumber);

            break;
        }
    }
}

vector<BasicPolyphonicAllocator::PolyVoice>& BasicPolyphonicAllocator::getVoices() {
	return voiceData;
}


int BasicPolyphonicAllocator::getNextVoice(int note)
{
    // Find a voice not playing any note
    if (inactiveVoiceQueue.size())
    {
        return inactiveVoiceQueue.front();
    }

    return -1;
}

int OldestNoteStealingPolyphonicAllocator::getNextVoice(int note)
{
    int voice = BasicPolyphonicAllocator::getNextVoice(note);
    if (voice >= 0)
        return voice;

    if (activeVoiceQueue.size())
    {
        return activeVoiceQueue.front();
    }

    return -1;
}

int LowestNoteStealingPolyphonicAllocator::getNextVoice(int note)
{
    int voice = BasicPolyphonicAllocator::getNextVoice(note);
    if (voice >= 0)
        return voice;

    // Find the playing voice with the lowest note that's lower than the requested note
    int lowestNote = note;
    int lowestVoice = -1;
    for (int voiceNumber : activeVoiceQueue)
    {
        PolyVoice& voice = voiceData[voiceNumber];
        if (voice.currentNote < lowestNote)
        {
            lowestNote = voice.currentNote;
            lowestVoice = voiceNumber;
        }
    }

    return lowestVoice;
}
