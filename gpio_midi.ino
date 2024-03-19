/*
 * This turns an arduino into a USB MIDI instrument.
 *
 * Author: Rudolf W Byker
 */

#include "MIDIUSB.h"
#include "midiUtil.h"
#include "notesUtil.h"
#include "analogMux.h"
#include "analogControl.h"


#define NOTE_VELOCITY 64
#define MIDI_CHANNEL 0

uint8_t numNotes = 0;
uint8_t numMuxes = 0;
uint8_t numAnalogControls = 0; // Maximum `numMuxes*8`
uint8_t numDigitalControls = 0; // TODO!

struct Note * notes;
struct AnalogMux8 * muxes;
struct AnalogControl * analogControls;
//struct DigitalControl digitalControls[NUM_DIGITAL_CONTROLS];  TODO!


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
  // printDigitalControls();  // TODO!
}


/**
 * Setup the Arduino for the upper ("swell") keyboard on Dolf's organ.
 */
void setupDolfOrganUpper() {
  // 61 notes on a 5-octave keyboard.
  numNotes = 61;
  notes = (struct Note*) malloc(numNotes * sizeof *notes);
  if (notes == NULL) {
    Serial.println("Error: Unable to allocate memory for notes.");
    Serial.flush();
    return;
  }
  createConsecutiveNotesInArray(
    2,  // Leave pins 0 and 1 open for the serial port.
    36,  // 36 is C2 in MIDI.
    numNotes,
    notes
  );
  initNotePins(notes, numNotes);
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
  notes = (struct Note*) malloc(numNotes * sizeof *notes);
  if (notes == NULL) {
    Serial.println("Error: Unable to allocate memory for notes.");
    Serial.flush();
    return;
  }
  createConsecutiveNotesInArray(
    2,  // Leave pins 0 and 1 open for the serial port.
    36,  // 36 is C2 in MIDI.
    numNotes,
    notes
  );
  initNotePins(notes, numNotes);

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
    1 // TODO?
  );
  for (i=0; i<numAnalogControls; i++) {
    analogControl_init(&analogControls[i]);
  }

  numDigitalControls = 0; // TODO!
}


void loop() {
  uint8_t i;

  // TODO: We probably need to read notes more frequently than controls.

  for (i=0; i<numNotes; i++) {
    mapNote(&notes[i], noteOn, noteOff);
  }

  for (i=0; i<numAnalogControls; i++) {
    analogControl_map(&analogControls[i], controlChange);
  }

  // for (i=0; i<numDigitalControls; i++) {
  //   TODO!
  // }

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
    Serial.print(notes[i].pitch);
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


void noteOn(const struct Note * const note) {
  MidiUSB.sendMIDI(MIDI_EVENT_NOTE_ON(MIDI_CHANNEL, note->pitch, NOTE_VELOCITY));
}


void noteOff(const struct Note * const note) {
  MidiUSB.sendMIDI(MIDI_EVENT_NOTE_OFF(MIDI_CHANNEL, note->pitch, NOTE_VELOCITY));
}


void controlChange(const struct AnalogControl * const control) {
  // MIDI control values are between 0 and 127 (7-bit unsigned).
  // The 12-bit ADC gives values between 0 and 4095 (12-bit unsigned).
  // We need to scale the ADC values to fit in the MIDI range.
  // We can do this by bit shifting (>> 5), which is the same as dividing by 32.
  MidiUSB.sendMIDI(MIDI_EVENT_CONTROL_CHANGE(MIDI_CHANNEL, control->number, control->value >> 5));
}
