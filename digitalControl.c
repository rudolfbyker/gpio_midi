#include "digitalControl.h"
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>


#define CONTROL_VALUE_MAX 4


struct DigitalControl createDigitalControl(
  uint8_t pin,
  uint8_t midiChannel,
  uint8_t midiControlNumber
) {
  pinMode(pin, INPUT_PULLUP);
  bool on = digitalRead(pin);
  
  return (struct DigitalControl){
    .pin=pin,
    .midiChannel=midiChannel,
    .midiControlNumber=midiControlNumber,
    .value=on,
    .on=on
  };
}


void createConsecutiveDigitalControlsInArray(
  struct DigitalControl * const controls,
  uint8_t numControls,
  uint8_t midiChannel,
  uint8_t firstMidiControlNumber,
  uint8_t firstPin
) {
  uint8_t i;
  for (i=0; i<numControls; i++) {
    controls[i] = createDigitalControl(
      firstPin + i,
      midiChannel,
      firstMidiControlNumber + i
    );
  }
}


void digitalControl_map(
  struct DigitalControl * self,
  void (*controlChange)(const struct DigitalControl * const control)
) {
  // TODO: This code is VERY similar to what we have in `mapNote`. Extract it to a common function?
  
  // Simulate a capacitor to debounce the pin.
  // If the pin is low, the switch is connected, and we increase the value.
  // If the pin is high, the switch is open, and we decrease the value.
  // We cap the value between 0 and CONTROL_VALUE_MAX.
  // When one of the limits are reached, we trigger the control on or off.
  bool pin = digitalRead(self->pin);

  if (pin) {
    // Switch is opening.
    if (self->value > 0) {
      self->value--;
    }
  } else {
    // Switch is closing.
    if (self->value < CONTROL_VALUE_MAX) {
      self->value++;
    }
  }

  if (self->on) {
    if (self->value == 0) {
      // Control should turn off.
      self->on = false;
      controlChange(self);
    }
  } else {
    if (self->value == CONTROL_VALUE_MAX) {
      // Control should turn on.
      self->on = true;
      controlChange(self);
    }
  }
}
