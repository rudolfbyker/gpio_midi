#include "analogControl.h"
#include "analogMux.h"
#include <stdint.h>


#define ANALOG_HYSTERESIS 32


void createConsecutiveAnalogControlsInArray(
  const struct AnalogMux8 * const muxes,
  uint8_t numMuxes,
  struct AnalogControl * const controls,
  uint8_t numControls,
  uint8_t midiChannel,
  uint8_t firstMidiControlNumber
) {
  uint8_t i;
  for (i=0; i<numControls; i++) {
    uint8_t muxIndex = i/8;
    uint8_t pinIndex = i%8;

    if (muxIndex >= numMuxes) {
      // Not enough muxes for the number of analog inputs!
      return;
    }

    controls[i] = (struct AnalogControl){
      .mux=&muxes[muxIndex],
      .selector=pinIndex,
      .channel=midiChannel,
      .number=firstMidiControlNumber + i,
      .value=0,
      .previousValue=0
    };
  }
}


void analogControl_init(
  struct AnalogControl * self
) {
  self->value = analogMux8_selectAndRead(self->mux, self->selector);
  self->previousValue = self->value;
}


void analogControl_map(
  struct AnalogControl * self,
  void (*controlChange)(const struct AnalogControl * const control)
) {
  self->value = analogMux8_selectAndRead(self->mux, self->selector);

  // Use hysteresis to avoid unnecessary updates, i.e., only emit a change if the value has changed enough.
  if (abs(self->value - self->previousValue) >= ANALOG_HYSTERESIS) {
    controlChange(self);
    self->previousValue = self->value;
  }
}
