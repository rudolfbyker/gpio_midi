#ifndef ANALOG_CONTROL_H
#define ANALOG_CONTROL_H

#include <stdint.h>
#include "analogMux.h"

#ifdef __cplusplus
extern "C" {
#endif

struct AnalogControl {
  // ADC Input
  struct AnalogMux8 * mux;
  uint8_t selector;

  // MIDI Output
  uint8_t channel;
  uint8_t number;

  // Runtime values
  uint16_t value;
  uint16_t previousValue;
};

void createConsecutiveAnalogControlsInArray(
  const struct AnalogMux8 * const muxes,
  uint8_t numMuxes,
  struct AnalogControl * const controls,
  uint8_t numControls,
  uint8_t midiChannel,
  uint8_t firstMidiControlNumber
);

void analogControl_map(
  struct AnalogControl * self,
  void (*controlChange)(const struct AnalogControl * const control)
);

void analogControl_init(
  struct AnalogControl * self
);

#ifdef __cplusplus
}
#endif

#endif
