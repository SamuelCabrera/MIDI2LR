#pragma once
/*
  ==============================================================================

    MIDIProcessor.h

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#ifndef MIDIPROCESSOR_H_INCLUDED
#define MIDIPROCESSOR_H_INCLUDED
#include <array>
#include <bitset>
#include "../JuceLibraryCode/JuceHeader.h"

class MIDICommandListener {
public:
  virtual void handleMidiCC(int midi_channel, int controller, int value) = 0;
  virtual void handleMidiNote(int midi_channel, int note) = 0;

  virtual ~MIDICommandListener() {};
};

class MIDIProcessor: public MidiInputCallback {
public:
  MIDIProcessor() noexcept;
  virtual ~MIDIProcessor();
  void Init(void);

  // overridden from MidiInputCallback
  void handleIncomingMidiMessage(MidiInput*, const MidiMessage&) override;

  void addMIDICommandListener(MIDICommandListener*);

  // re-enumerates MIDI IN devices
  void rescanDevices();

private:
  void InitDevices_();
  Array<MIDICommandListener *> listeners_;
  OwnedArray<MidiInput> devices_;
  juce::MidiRPNDetector nrpn_detector_;
  std::bitset<16> processing_nrpn_; //default init false
  std::array<juce::MidiRPNMessage, 16> nrpn_messages_{{
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true},
    {0,0,0,true,true}
    }};//double braces needed in C++11!
};

#endif  // MIDIPROCESSOR_H_INCLUDED
