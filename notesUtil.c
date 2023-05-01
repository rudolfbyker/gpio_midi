#include "notesUtil.h"
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>


#define NOTE_VALUE_MAX 4


void initNotePins(struct Note *notes, uint8_t numNotes) {
  uint8_t i;
  for (i=0; i<numNotes; i++) {
    pinMode(notes[i].pin, INPUT_PULLUP);
  }
}


struct Note createNote(uint8_t pin, uint8_t pitch) {
  return (struct Note){
    .pin=pin,
    .pitch=pitch,
    .value=0,
    .on=false
  };
}


void createConsecutiveNotesInArray(
  uint8_t firstPin,
  uint8_t firstPitch,
  uint8_t numNotes,
  struct Note *destination
) {
  uint8_t i;
  for (i=0; i<numNotes; i++) {
    destination[i] = createNote(
      firstPin + i,
      firstPitch + i
    );
  }
}


void mapNote(
  struct Note * note,
  void (*noteOn)(const struct Note * const note),
  void (*noteOff)(const struct Note * const note)
) {
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
      noteOff(note);
      note->on = false;
    }
  } else {
    // Note is not playing.

    if (note->value == NOTE_VALUE_MAX) {
      // Note should start playing.
      noteOn(note);
      note->on = true;
    }
  }
}
