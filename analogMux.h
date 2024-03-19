/**
 * From https://github.com/rudolfbyker/analog-multiplexer
 */

#ifndef ANALOG_MUX_H
#define ANALOG_MUX_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct AnalogMux8 {
  uint8_t selectorPin0; // Least significant bit.
  uint8_t selectorPin1;
  uint8_t selectorPin2; // Most significant bit.

  uint8_t analogPin; // Where we read or write the value.
  uint8_t analogPinDirection; // OUTPUT=1, INPUT=0

  uint16_t delayAfterSelection; // Delay between selection and using the analog pin to allow the mux to settle.
};

void analogMux8_init(const struct AnalogMux8 *self);

uint32_t analogMux8_selectAndRead(const struct AnalogMux8 *self, uint8_t selector);

void analogMux8_selectAndWrite(const struct AnalogMux8 *self, uint8_t selector, uint32_t value);

void analogMux8_select(const struct AnalogMux8 *self, uint8_t selector);

void createConsecutiveMuxesInArray(
 uint8_t firstSelectorPin,
 uint8_t firstAnalogPin,
 uint8_t analogPinDirection,
 uint16_t delayAfterSelection,
 uint8_t numMuxes,
 struct AnalogMux8 *const muxes
);

#ifdef __cplusplus
}
#endif

#endif
