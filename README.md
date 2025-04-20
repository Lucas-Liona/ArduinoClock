# This is a clock made with Arduino, complete with a joystick for user input, an LCD screen, and a RTC component to tell the time

## Key Features
- Set/Disable Alarms in setting menu
- Disarm Alarms by following uniquely generated sequence

## Although this is simplistic in design, it takes a lot of code to have a polished UI, the cursor intelligently only allows the user to hover selectable slots which we predefine.

- Main.ino contains the inital code, which works but is not optimized
- V2.ino runs the 'same', but is optimized for memory/runtime/battery consumption
  - Does not constantly refresh unless necessary
  - Easier to read code
  - Memory is handled more precisely

## Further/Future improvements
- AM/FM radio
- More automation, i.e. opening blinds, turning on lights, running coffee machine, etc.
- 3D printed shell and custom compact circuitry
