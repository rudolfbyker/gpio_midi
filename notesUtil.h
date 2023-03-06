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

#define NUM_NOTES 54

struct Note * getNotes();
void initNotePins(struct Note *notes, uint8_t numNotes);

#ifdef __cplusplus
}
#endif

#endif
