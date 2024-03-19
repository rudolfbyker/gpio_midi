/**
 * From https://github.com/rudolfbyker/analog-multiplexer
 */

#include <stdint.h>
#include "analogMux.h"

void analogMux8_init(const struct AnalogMux8 *const self) {
  pinMode(self->selectorPin0, 1); // OUTPUT
  pinMode(self->selectorPin1, 1); // OUTPUT
  pinMode(self->selectorPin2, 1); // OUTPUT
  pinMode(self->analogPin, self->analogPinDirection);
}

uint32_t analogMux8_selectAndRead(const struct AnalogMux8 *const self, uint8_t selector) {
  analogMux8_select(self, selector);
  delay(self->delayAfterSelection);
  return analogRead(self->analogPin);
}

void analogMux8_selectAndWrite(const struct AnalogMux8 *const self, uint8_t selector, uint32_t value) {
  analogMux8_select(self, selector);
  delay(self->delayAfterSelection);
  return analogWrite(self->analogPin, value);
}

void analogMux8_select(const struct AnalogMux8 *const self, uint8_t selector) {
  digitalWrite(self->selectorPin0, (selector & (1 << 0)) >> 0);
  digitalWrite(self->selectorPin1, (selector & (1 << 1)) >> 1);
  digitalWrite(self->selectorPin2, (selector & (1 << 2)) >> 2);
}

void createConsecutiveMuxesInArray(
 uint8_t firstSelectorPin,
 uint8_t firstAnalogPin,
 uint8_t analogPinDirection,
 uint16_t delayAfterSelection,
 uint8_t numMuxes,
 struct AnalogMux8 *const muxes
) {
  uint8_t i;
  for (i=0; i<numMuxes; i++) {
    muxes[i].selectorPin0 = firstSelectorPin + 3 * i;
    muxes[i].selectorPin1 = firstSelectorPin + 3 * i + 1;
    muxes[i].selectorPin2 = firstSelectorPin + 3 * i + 2;
    muxes[i].analogPin = firstAnalogPin + i;
    muxes[i].analogPinDirection = analogPinDirection;
    muxes[i].delayAfterSelection = delayAfterSelection;
  }
}
