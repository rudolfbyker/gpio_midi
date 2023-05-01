/*
 * This turns an arduino into a USB MIDI instrument.
 *
 * Author: Rudolf W Byker
 */

#include "MIDIUSB.h"
#include "midiUtil.h"
#include "notesUtil.h"


#define NOTE_VELOCITY 64
#define MIDI_CHANNEL 0

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
    mapNote(&notes[i], noteOn, noteOff);
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


void noteOn(const struct Note * const note) {
  MidiUSB.sendMIDI(MIDI_EVENT_NOTE_ON(MIDI_CHANNEL, note->pitch, NOTE_VELOCITY));
}


void noteOff(const struct Note * const note) {
  MidiUSB.sendMIDI(MIDI_EVENT_NOTE_OFF(MIDI_CHANNEL, note->pitch, NOTE_VELOCITY));
}


void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
  MidiUSB.sendMIDI(MIDI_EVENT_CONTROL_CHANGE(channel, control, value));
}
