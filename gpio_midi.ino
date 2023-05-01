/*
 * This turns an arduino into a USB MIDI instrument.
 *
 * Author: Rudolf W Byker
 * Based on code by gurbrinder grewal and Arduino LLC.
 */

#include "MIDIUSB.h"
#include "midiUtil.h"
#include "notesUtil.h"


#define NOTE_VELOCITY 64
#define NOTE_VALUE_MAX 4

// 61 notes on a 5-octave keyboard.
#define NUM_NOTES 61

// Leave pins 0 and 1 open for the serial port.
#define FIRST_PIN 2

// 36 is C2 in MIDI.
#define FIRST_PITCH 36

struct Note notes[NUM_NOTES];

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  createConsecutiveNotesInArray(FIRST_PIN, FIRST_PITCH, NUM_NOTES, notes);
  printNotes();
  initNotePins(notes, NUM_NOTES);
}


void loop() {
  uint8_t i;
  for (i=0; i<NUM_NOTES; i++) {
    mapNote(&notes[i], 0);
  }
  MidiUSB.flush();
}


void printNotes() {
  Serial.println("<notes>");
  Serial.println("i,pin,pitch");
  uint8_t i;
  for (i=0; i<NUM_NOTES; i++) {
    Serial.print(i);
    Serial.print(",");
    Serial.print(notes[i].pin);
    Serial.print(",");
    Serial.print(notes[i].pitch);
    Serial.println();
  }
  Serial.println("</notes>");
  Serial.flush();
}


void mapNote(struct Note * note, uint8_t channel) {
  // Simulate a capacitor to debounce the pin.
  // If the pin is low, the switch is connected, and we increase the value.
  // If the pin is high, the switch is open, and we decrease the value.
  // We cap the value between 0 and NOTE_VALUE_MAX.
  // When one of the limits are reached, we trigger the note on or off.
  bool pin = digitalRead(note->pin);

  if (pin) {
    // Switch is opening.
    if (note->value > 0) {
      note->value--;
    }
  } else {
    // Switch is closing.
    if (note->value < NOTE_VALUE_MAX) {
      note->value++;
    }
  }

  if (note->on) {
    // Note is playing.

    if (note->value == 0) {
      // Note should stop playing.
      noteOff(channel, note->pitch, NOTE_VELOCITY);
      note->on = false;
    }
  } else {
    // Note is not playing.

    if (note->value == NOTE_VALUE_MAX) {
      // Note should start playing.
      noteOn(channel, note->pitch, NOTE_VELOCITY);
      note->on = true;
    }
  }
}


void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MidiUSB.sendMIDI(MIDI_EVENT_NOTE_ON(channel, pitch, velocity));
}


void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  MidiUSB.sendMIDI(MIDI_EVENT_NOTE_OFF(channel, pitch, velocity));
}


void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
  MidiUSB.sendMIDI(MIDI_EVENT_CONTROL_CHANGE(channel, control, value));
}
