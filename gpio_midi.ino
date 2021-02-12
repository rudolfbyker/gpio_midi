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
  byte value;
  bool on;
};


#define NUM_NOTES 54
Note notes[NUM_NOTES];

void setup() {
  notes[0] = (struct Note){0, 53, 0, false}; // F3
  notes[1] = (struct Note){1, 54, 0, false};
  notes[2] = (struct Note){2, 55, 0, false};
  notes[3] = (struct Note){3, 56, 0, false};
  notes[4] = (struct Note){4, 57, 0, false};
  notes[5] = (struct Note){5, 58, 0, false};
  notes[6] = (struct Note){6, 59, 0, false};
  notes[7] = (struct Note){7, 60, 0, false};
  notes[8] = (struct Note){8, 61, 0, false};
  notes[9] = (struct Note){9, 62, 0, false};
  notes[10] = (struct Note){10, 63, 0, false};
  notes[11] = (struct Note){11, 64, 0, false};
  notes[12] = (struct Note){12, 65, 0, false};
  notes[13] = (struct Note){13, 66, 0, false};
  notes[14] = (struct Note){14, 67, 0, false};
  notes[15] = (struct Note){15, 68, 0, false};
  notes[16] = (struct Note){16, 69, 0, false};
  notes[17] = (struct Note){17, 70, 0, false};
  notes[18] = (struct Note){18, 71, 0, false};
  notes[19] = (struct Note){19, 72, 0, false};
  notes[20] = (struct Note){20, 73, 0, false};
  notes[21] = (struct Note){21, 74, 0, false};
  notes[22] = (struct Note){22, 75, 0, false};
  notes[23] = (struct Note){23, 76, 0, false};
  notes[24] = (struct Note){24, 77, 0, false};
  notes[25] = (struct Note){25, 78, 0, false};
  notes[26] = (struct Note){26, 79, 0, false};
  notes[27] = (struct Note){27, 80, 0, false};
  notes[28] = (struct Note){28, 81, 0, false};
  notes[29] = (struct Note){29, 82, 0, false};
  notes[30] = (struct Note){30, 83, 0, false};
  notes[31] = (struct Note){31, 84, 0, false};
  notes[32] = (struct Note){32, 85, 0, false};
  notes[33] = (struct Note){33, 86, 0, false};
  notes[34] = (struct Note){34, 87, 0, false};
  notes[35] = (struct Note){35, 88, 0, false};
  notes[36] = (struct Note){36, 89, 0, false};
  notes[37] = (struct Note){37, 90, 0, false};
  notes[38] = (struct Note){38, 91, 0, false};
  notes[39] = (struct Note){39, 92, 0, false};
  notes[40] = (struct Note){40, 93, 0, false};
  notes[41] = (struct Note){41, 94, 0, false};
  notes[42] = (struct Note){42, 95, 0, false};
  notes[43] = (struct Note){43, 96, 0, false};
  notes[44] = (struct Note){44, 97, 0, false};
  notes[45] = (struct Note){45, 98, 0, false};
  notes[46] = (struct Note){46, 100, 0, false};
  notes[47] = (struct Note){47, 101, 0, false};
  notes[48] = (struct Note){48, 102, 0, false};
  notes[49] = (struct Note){49, 103, 0, false};
  notes[50] = (struct Note){50, 104, 0, false};
  notes[51] = (struct Note){51, 105, 0, false};
  notes[52] = (struct Note){52, 106, 0, false};
  notes[53] = (struct Note){53, 107, 0, false};

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
const byte maxValue = 4;
void map(struct Note * note, byte channel) {
  // Simulate a capacitor to debounce the pin.
  // If the pin is low, the switch is connected, and we increase the value.
  // If the pin is high, the switch is open, and we decrease the value.
  // We cap the value between 0 and maxValue.
  // When one of the thresholds are reached, we trigger the note on or off.
  bool pin = digitalRead(note->pin);

  if (pin) {
    // Switch is opening.
    if (note->value > 0) {
      note->value--;
    }
  } else {
    // Switch is closing.
    if (note->value < maxValue) {
      note->value++;
    }
  }

  if (note->on) {
    // Note is playing.

    if (note->value == 0) {
      // Note should stop playing.
      noteOff(channel, note->pitch, velocity);
      note->on = false;
    }
  } else {
    // Note is not playing.

    if (note->value == maxValue) {
      // Note should start playing.
      noteOn(channel, note->pitch, velocity);
      note->on = true;
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
