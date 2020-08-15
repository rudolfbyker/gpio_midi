/*
 * This turns an arduino into a USB MIDI instrument.
 * 
 * Author: Rudolf W Byker
 * Based on code by gurbrinder grewal and Arduino LLC.
 */ 

#include "MIDIUSB.h"

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}


struct Note {
  byte pin;
  byte pitch;
  bool lastValue;
};


#define NUM_NOTES 54
Note notes[NUM_NOTES];

void setup() {
  notes[0] = (struct Note){0, 21, false};
  notes[1] = (struct Note){1, 22, false};
  notes[2] = (struct Note){2, 23, false};
  notes[3] = (struct Note){3, 24, false};
  notes[4] = (struct Note){4, 25, false};
  notes[5] = (struct Note){5, 26, false};
  notes[6] = (struct Note){6, 27, false};
  notes[7] = (struct Note){7, 28, false};
  notes[8] = (struct Note){8, 29, false};
  notes[9] = (struct Note){9, 30, false};
  notes[10] = (struct Note){10, 31, false};
  notes[11] = (struct Note){11, 32, false};
  notes[12] = (struct Note){12, 33, false};
  notes[13] = (struct Note){13, 34, false};
  notes[14] = (struct Note){14, 35, false};
  notes[15] = (struct Note){15, 36, false};
  notes[16] = (struct Note){16, 37, false};
  notes[17] = (struct Note){17, 38, false};
  notes[18] = (struct Note){18, 39, false};
  notes[19] = (struct Note){19, 40, false};
  notes[20] = (struct Note){20, 41, false};
  notes[21] = (struct Note){21, 42, false};
  notes[22] = (struct Note){22, 43, false};
  notes[23] = (struct Note){23, 44, false};
  notes[24] = (struct Note){24, 45, false};
  notes[25] = (struct Note){25, 46, false};
  notes[26] = (struct Note){26, 47, false};
  notes[27] = (struct Note){27, 48, false};
  notes[28] = (struct Note){28, 49, false};
  notes[29] = (struct Note){29, 50, false};
  notes[30] = (struct Note){30, 51, false};
  notes[31] = (struct Note){31, 52, false};
  notes[32] = (struct Note){32, 53, false};
  notes[33] = (struct Note){33, 54, false};
  notes[34] = (struct Note){34, 55, false};
  notes[35] = (struct Note){35, 56, false};
  notes[36] = (struct Note){36, 57, false};
  notes[37] = (struct Note){37, 58, false};
  notes[38] = (struct Note){38, 59, false};
  notes[39] = (struct Note){39, 60, false};
  notes[40] = (struct Note){40, 61, false};
  notes[41] = (struct Note){41, 62, false};
  notes[42] = (struct Note){42, 63, false};
  notes[43] = (struct Note){43, 64, false};
  notes[44] = (struct Note){44, 65, false};
  notes[45] = (struct Note){45, 66, false};
  notes[46] = (struct Note){46, 67, false};
  notes[47] = (struct Note){47, 68, false};
  notes[48] = (struct Note){48, 69, false};
  notes[49] = (struct Note){49, 70, false};
  notes[50] = (struct Note){50, 71, false};
  notes[51] = (struct Note){51, 72, false};
  notes[52] = (struct Note){52, 73, false};
  notes[53] = (struct Note){53, 74, false};

  byte i;
  for (i=0; i<NUM_NOTES; i++) {
    pinMode(notes[i].pin, INPUT_PULLUP);
  }
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

const byte velocity = 64;
void map(struct Note * note, byte channel) {
  if (note->lastValue) {
    // Pin was high. Read new value.
    note->lastValue = digitalRead(note->pin);
    if (!note->lastValue) {
      // Pin fell. Play note.
      noteOn(channel, note->pitch, velocity);
    }
  }
  else {
    // Pin was low. Read new value.
    note->lastValue = digitalRead(note->pin);
    if (note->lastValue) {
      // Pin rose. Stop note.
      noteOff(channel, note->pitch, velocity);
    }
  }
}

void loop() {
  byte i;
  for (i=0; i<NUM_NOTES; i++) {
    map(&notes[i], 0);
  }
  MidiUSB.flush();
}
