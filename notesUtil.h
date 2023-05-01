#ifndef NOTES_UTIL_H
#define NOTES_UTIL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Note {
  uint8_t pin;
  uint8_t pitch;
  uint8_t value;
  bool on;
};

void initNotePins(struct Note *notes, uint8_t numNotes);

struct Note createNote(uint8_t pin, uint8_t pitch);

void createConsecutiveNotesInArray(uint8_t firstPin, uint8_t firstPitch, uint8_t numNotes, struct Note *destination);

void mapNote(
  struct Note * note,
  void (*noteOn)(const struct Note * const note),
  void (*noteOff)(const struct Note * const note)
);

#ifdef __cplusplus
}
#endif

#endif
