#ifndef MIDI_UTIL_H
#define MIDI_UTIL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).
#define MIDI_EVENT_NOTE_ON(CHANNEL, PITCH, VELOCITY) {0x09, 0x90 | CHANNEL, PITCH, VELOCITY}
#define MIDI_EVENT_NOTE_OFF(CHANNEL, PITCH, VELOCITY) {0x08, 0x80 | CHANNEL, PITCH, VELOCITY}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).
#define MIDI_EVENT_CONTROL_CHANGE(CHANNEL, CONTROL, VALUE) {0x0B, 0xB0 | CHANNEL, CONTROL, VALUE}

#ifdef __cplusplus
}
#endif

#endif
