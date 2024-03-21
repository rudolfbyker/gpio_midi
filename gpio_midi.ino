/*
 * This turns an arduino into a USB MIDI instrument.
 *
 * Author: Rudolf W Byker
 */

#include "MIDIUSB.h"
#include "midiUtil.h"
#include "analogMux.h"
#include "analogControl.h"
#include "digitalControl.h"


#define NOTE_VELOCITY 64
#define MIDI_CHANNEL 0

uint8_t numNotes = 0;
uint8_t numMuxes = 0;
uint8_t numAnalogControls = 0; // Maximum `numMuxes*8`
uint8_t numDigitalControls = 0;

struct DigitalControl * notes;
struct AnalogMux8 * muxes;
struct AnalogControl * analogControls;
struct DigitalControl * digitalControls;


void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  // Arduino Due has 12-bit ADCs.
  analogReadResolution(12);

  // Choose one, or make your own:
//   setupDolfOrganUpper();
//   setupDolfOrganLower();
  setupDolfOrganPedal();

  printNotes();
  printAnalogControls();
  printDigitalControls();
}


/**
 * Setup the Arduino for the upper ("swell") keyboard on Dolf's organ.
 */
void setupDolfOrganUpper() {
  // 61 notes on a 5-octave keyboard.
  numNotes = 61;
  notes = (struct DigitalControl*) malloc(numNotes * sizeof *notes);
  if (notes == NULL) {
    Serial.println("Error: Unable to allocate memory for notes.");
    Serial.flush();
    return;
  }
  createConsecutiveDigitalControlsInArray(
    notes,
    numNotes,
    MIDI_CHANNEL,
    36,  // Note number 36 is C2 in MIDI.
    2  // Leave pins 0 and 1 open for the serial port.
  );
}


/**
 * Setup the Arduino for the lower ("great") keyboard on Dolf's organ.
 */
void setupDolfOrganLower() {
  // Identical to Upper.
  setupDolfOrganUpper();
}


/**
 * Setup the Arduino for the pedals on Dolf's organ.
 */
void setupDolfOrganPedal() {
  // 25 notes on a 3-octave pedal.
  numNotes = 25;
  notes = (struct DigitalControl*) malloc(numNotes * sizeof *notes);
  if (notes == NULL) {
    Serial.println("Error: Unable to allocate memory for notes.");
    Serial.flush();
    return;
  }
  createConsecutiveDigitalControlsInArray(
    notes,
    numNotes,
    MIDI_CHANNEL,
    36,  // Note number 36 is C2 in MIDI.
    2  // Leave pins 0 and 1 open for the serial port.
  );

  // Because my pedal is only 3 octaves, I have many open pins for analog and digital controls on this arduino.

  // Each 8-bit mux requires 3 selector pins.
  // The analog pins are consecutive, luckily, so A0+1 = A1, etc.
  numMuxes = 6;
  muxes = (struct AnalogMux8*) malloc(numMuxes * sizeof *muxes);
  if (muxes == NULL) {
    Serial.println("Error: Unable to allocate memory for muxes.");
    Serial.flush();
    return;
  }
  createConsecutiveMuxesInArray(
    // Mux1 to Mux3 use D45 to D53 as selector pins and A0 to A2 as analog input pins.
    45,
    A0,
    INPUT,
    0,
    3,
    muxes
  );
  createConsecutiveMuxesInArray(
    // Mux4 to Mux6 use D61 to D69 as selector pins and A3 to A5 as analog input pins.
    61,
    A3,
    INPUT,
    0,
    3,
    &muxes[3]
  );
  uint8_t i;
  for (i=0; i<numMuxes; i++) {
    analogMux8_init(&muxes[i]);
  }

  // 6 Muxes provide 48 analog inputs, but we use fewer.
  numAnalogControls = 44;
  analogControls = (struct AnalogControl*) malloc(numAnalogControls * sizeof *analogControls);
  if (analogControls == NULL) {
    Serial.println("Error: Unable to allocate memory for analog controls.");
    Serial.flush();
    return;
  }
  createConsecutiveAnalogControlsInArray(
    muxes,
    numMuxes,
    analogControls,
    numAnalogControls,
    MIDI_CHANNEL,
    0
  );
  // Reassign control from MIDI control number 6 to 65, because Hauptwerk does not respond to 6 for some unknown reason.
  analogControls[6].number = 65;
  for (i=0; i<numAnalogControls; i++) {
    analogControl_init(&analogControls[i]);
  }

  numDigitalControls = 2;
  digitalControls = (struct DigitalControl*) malloc(numDigitalControls * sizeof *digitalControls);
  if (digitalControls == NULL) {
    Serial.println("Error: Unable to allocate memory for digital controls.");
    Serial.flush();
    return;
  }
  createConsecutiveDigitalControlsInArray(
    digitalControls,
    numDigitalControls,
    MIDI_CHANNEL,
    48,
    27
  );
}


void loop() {
  uint8_t i;

  // TODO: We probably need to read notes more frequently than controls.

  for (i=0; i<numNotes; i++) {
    digitalControl_map(&notes[i], noteChange);
  }

  for (i=0; i<numAnalogControls; i++) {
    analogControl_map(&analogControls[i], analogControlChange);
  }

  for (i=0; i<numDigitalControls; i++) {
    digitalControl_map(&digitalControls[i], digitalControlChange);
  }

  MidiUSB.flush();
}


void printNotes() {
  Serial.println("<notes>");
  Serial.println("i,pin,pitch");
  uint8_t i;
  for (i=0; i<numNotes; i++) {
    Serial.print(i);
    Serial.print(",");
    Serial.print(notes[i].pin);
    Serial.print(",");
    Serial.print(notes[i].midiControlNumber);
    Serial.println();
  }
  Serial.println("</notes>");
  Serial.flush();
}


void printAnalogControls() {
  Serial.println("<analogControls>");
  Serial.println("i,analogPin,channel,number,selector,value");
  uint8_t i;
  for (i=0; i<numAnalogControls; i++) {
    Serial.print(i);
    Serial.print(",");
    Serial.print(analogControls[i].mux->analogPin);
    Serial.print(",");
    Serial.print(analogControls[i].channel);
    Serial.print(",");
    Serial.print(analogControls[i].number);
    Serial.print(",");
    Serial.print(analogControls[i].selector);
    Serial.print(",");
    Serial.print(analogControls[i].value);
    Serial.println();
  }
  Serial.println("</analogControls>");
  Serial.flush();
}


void printDigitalControls() {
  Serial.println("<digitalControls>");
  Serial.println("i,pin,midiChannel,midiControlNumber,value,on");
  uint8_t i;
  for (i=0; i<numDigitalControls; i++) {
    Serial.print(i);
    Serial.print(",");
    Serial.print(digitalControls[i].pin);
    Serial.print(",");
    Serial.print(digitalControls[i].midiChannel);
    Serial.print(",");
    Serial.print(digitalControls[i].midiControlNumber);
    Serial.print(",");
    Serial.print(digitalControls[i].value);
    Serial.print(",");
    Serial.print(digitalControls[i].on);
    Serial.println();
  }
  Serial.println("</digitalControls>");
  Serial.flush();
}


void noteChange(const struct DigitalControl * const control) {
  if (control->on) {
    MidiUSB.sendMIDI(MIDI_EVENT_NOTE_ON(control->midiChannel, control->midiControlNumber, NOTE_VELOCITY));
  } else {
    MidiUSB.sendMIDI(MIDI_EVENT_NOTE_OFF(control->midiChannel, control->midiControlNumber, NOTE_VELOCITY));
  }
}


void analogControlChange(const struct AnalogControl * const control) {
  // MIDI control values are between 0 and 127 (7-bit unsigned).
  // The 12-bit ADC gives values between 0 and 4095 (12-bit unsigned).
  // We need to scale the ADC values to fit in the MIDI range.
  // We can do this by bit shifting (>> 5), which is the same as dividing by 32.
  MidiUSB.sendMIDI(MIDI_EVENT_CONTROL_CHANGE(control->channel, control->number, control->value >> 5));
}


void digitalControlChange(const struct DigitalControl * const control) {
  // MIDI control values are between 0 and 127 (7-bit unsigned).
  // Digital controls are either on or off.
  MidiUSB.sendMIDI(MIDI_EVENT_CONTROL_CHANGE(control->midiChannel, control->midiControlNumber, control->on ? 127 : 0));
}
