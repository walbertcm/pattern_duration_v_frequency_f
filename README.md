# pattern_duration_v_frequency_f
 Pattern Duration variable and Frequency Fixed

# Vibrotactile Perception Tasks

This Arduino project contains a complete implementation of a multi-phase perceptual experiment using 8 vibration motors and 8 physical buttons. Participants are exposed to vibrotactile stimuli and must complete different tasks involving identification, comparison, and interactive reordering of vibration pulses.

## Features
- 3 experimental tasks:
  - **Identify Shortest**: Detect the motor with the shortest pulse duration.
  - **Identify Longest**: Detect the motor with the longest pulse duration.
  - **Sort by Swapping**: Reorder the motors from shortest to longest by selecting pairs of buttons to swap durations.
- Randomized stimulus presentation
- Real-time vibration feedback using PWM
- Serial monitor logging of all responses and performance
- Pause and resume between tasks

## Hardware Requirements
- Arduino Mega 2560 (recommended for pin availability)
- 8 LRA or ERM vibration motors (e.g., LRA1027)
- 8 push buttons (momentary)
- Resistors (for pull-up configuration if needed)
- External power supply (for motors)
- Breadboard and jumper wires

## How to Use
1. Connect 8 vibration motors to PWM-capable pins and buttons to digital pins as specified in the code.
2. Upload the sketch to your Arduino board.
3. Open the Serial Monitor at 9600 baud rate.
4. Follow the on-screen instructions:
   - Press the button corresponding to the motor you perceive as correct (shortest/longest).
   - In the sort task, click two buttons to swap values. Double click button 7 to finish.
5. The results for each task are printed in the serial log.

## File Structure
- `ordenar_valores_arduino.ino` – Main Arduino sketch with all task implementations.
- `README.md` – This documentation file.

## License
This project is licensed under the MIT License. See the LICENSE file for details.

## Authors
Developed by [Your Name or Research Group] as part of a research project in haptic data representation.

---
Feel free to adapt, expand, or translate for your specific experimental needs.

