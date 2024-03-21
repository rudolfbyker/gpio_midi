#ifndef DIGITAL_CONTROL_H
#define DIGITAL_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct DigitalControl {
  // Digital input
  uint8_t pin;

  // MIDI Output
  uint8_t midiChannel;
  uint8_t midiControlNumber;

  // Runtime values
  uint8_t value;
  bool on;
};

struct DigitalControl createDigitalControl(
  uint8_t pin,
  uint8_t midiChannel,
  uint8_t midiControlNumber
);

void createConsecutiveDigitalControlsInArray(
  struct DigitalControl * const controls,
  uint8_t numControls,
  uint8_t midiChannel,
  uint8_t firstMidiControlNumber,
  uint8_t firstPin
);

void digitalControl_map(
  struct DigitalControl * self,
  void (*controlChange)(const struct DigitalControl * const control)
);

#ifdef __cplusplus
}
#endif

#endif
