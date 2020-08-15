# gpio_midi

Turn an Arduino into a USB MIDI instrument. Full polyphony is supported, and latency is imperceptible.

## Instructions

- Get the [Arduino IDE](https://www.arduino.cc/en/main/software).
- Get the [MIDIUSB Library for Arduino](https://github.com/arduino-libraries/MIDIUSB)
- Get an Arduino that is compatible with the MIDIUSB Library. I like to use the Arduino Due. 
For users in South Africa, I recommend [these clones from DIY Electronics](https://www.diyelectronics.co.za/store/boards/1437-arduino-due.html).
- Upload the sketch to the Arduino.
- Somehow cause your instrument to connect GPIO pins to ground.
For each GPIO pin that is connected to ground, a note will play.
Each GPIO pin corresponds to a MIDI note number.
[Each MIDI note number corresponds to a pitch](https://computermusicresource.com/midikeys.html).
These can be modified by editing the `setup()` function in the sketch.
