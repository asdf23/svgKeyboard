# SVG Keyboard

## What is this?

The SVG Keyboard is an open-source project designed to help craft SVG animations. It consists of 12 buttons (8 rotary dials that can also be pressed like buttons, and 4 standard buttons) and 8 dials. Each input can be mapped to:

- A keyboard key
- A string of characters
- Mouse movement
- Mouse button presses

Every part of this project is open source, including:

- STL files for 3D-printing the case
- PCB schematics
- Microcontroller source code
- Software for SVG editing (the keyboard's original purpose)

The SVG Keyboard was designed to provide a physical interface for quickly altering shapes without the distractions of a traditional keyboard or mouse.



https://github.com/user-attachments/assets/6bbcc28e-a6bd-4fad-a9b8-0a243eec3b54


---

## How do you use it?

Plug it in, and start typing away! (err "turning away"?)

While the keyboard is intended for use with custom SVG editing software, it can be reconfigured for other purposes, including general use as a keyboard or mouse.

---

## How do you reprogram it?

There are two ways to reprogram the SVG Keyboard, depending on your goals:

1. **Change the Behavior of Dials and Buttons**  
   To remap actions:
   - Save a file named `config.txt` to the root of an SD card.
   - Define the mapping for each physical interaction in this file. Actions can include keyboard inputs, strings of characters, mouse movements, or mouse button presses.

2. **Update the Microcontroller Software**  
   The microcontroller is not locked, and you are free to update its software. The project is licensed under GPL, so collaboration is encouraged! Share your improvements and let's make it better together. To reprogram the microcontroller, you'll need:
   - [Arduino IDE](https://www.arduino.cc/en/software)
   - Necessary Arduino libraries
   - Teensy 4.1 library

---

## Frequently Asked Questions

### Can I use this as a music or MIDI device?
Not yet, but it's possible! It would require flashing the microcontroller with MIDI-compatible software. This feature is on the to-do list. The plan is to enable MIDI events so the device can interact with software directly, by passing keyboard events completely. I would guess that your purpose is for music or lighting control. I'm not too familar with this. Would appreciate you help.

### Can I use it like a regular keyboard or mouse?
Absolutely! If your goal is to type out something like "ARRRRRRGGGGHHH!!!" it would be well suited to this task. But seriously you could type an email or use it for gaming input. It can function like a standard keyboard / mouse.

---

## License

This project is licensed under the [GNU General Public License (GPL)](https://www.gnu.org/licenses/gpl-3.0.en.html). You are free to use, modify, and share it, but please share your contributions so we can improve the project together!

