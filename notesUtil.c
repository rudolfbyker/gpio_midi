#include "notesUtil.h"
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>

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

void createConsecutiveNotesInArray(uint8_t firstPin, uint8_t firstPitch, uint8_t numNotes, struct Note *destination) {
  uint8_t i;
  for (i=0; i<numNotes; i++) {
    destination[i] = createNote(
      firstPin + i,
      firstPitch + i
    );
  }
}
