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
   - <details>
      
      <summary>Key events that are mappable</summary>
      
        If an SD card is inserted and it contains a config file in the root directory named `config.txt`, then the default mappings are replaced. The format of the file is a key/value pair deliminted by an equals (=) sign and separated by newlines. The left hand side should be one or more of the following:
        
        ```
        SWITCH_INCREASE=
        SWITCH_DECREASE=
        SWITCH_PREVIOUS=
        SWITCH_NEXT=
        ROTARY_SVG_WIDTH_CLICK=
        ROTARY_VIEWBOX_WIDTH_CLICK=
        ROTARY_SVG_Y_CLICK=
        ROTARY_VIEWBOX_Y_CLICK=
        ROTARY_VIEWBOX_HEIGHT_CLICK=
        ROTARY_SVG_HEIGHT_CLICK=
        ROTARY_VIEWBOX_X_CLICK=
        ROTARY_SVG_X_CLICK=
        ROTARY_SVG_WIDTH_LEFT=
        ROTARY_SVG_WIDTH_RIGHT=
        ROTARY_VIEWBOX_WIDTH_LEFT=
        ROTARY_VIEWBOX_WIDTH_RIGHT=
        ROTARY_SVG_Y_LEFT=
        ROTARY_SVG_Y_RIGHT=
        ROTARY_VIEWBOX_Y_LEFT=
        ROTARY_VIEWBOX_Y_RIGHT=
        ROTARY_VIEWBOX_HEIGHT_LEFT=
        ROTARY_VIEWBOX_HEIGHT_RIGHT=
        ROTARY_SVG_HEIGHT_LEFT=
        ROTARY_SVG_HEIGHT_RIGHT=
        ROTARY_VIEWBOX_X_LEFT=
        ROTARY_VIEWBOX_X_RIGHT=
        ROTARY_SVG_X_LEFT=
        ROTARY_SVG_X_RIGHT=
        ```
        
   </details>

   - <details>
      
      <summary>Mapping a string</summary>
        
        The right hand side can be any string except an escaped character sequence
        
        ```
        SWITCH_INCREASE=Hello World
        SWITCH_DECREASE=Type any string 
        ```
        
   </details>

   - <details>
      
      <summary>Mapping a mouse click</summary>
        
        Mouse click events are supported
        
        ```
        SWITCH_INCREASE={MOUSE_LEFT}
        
        {MOUSE_LEFT}
        {MOUSE_RIGHT}
        {MOUSE_MIDDLE}
        ```
        
   </details>

   - <details>
      
      <summary>Mapping a mouse movement</summary>
        
        Mouse move events are supported
        
        ```
        ROTARY_VIEWBOX_Y_RIGHT={MOUSE_MOVE_X_DECREMENT}
        
        {MOUSE_MOVE_X_DECREMENT}
        {MOUSE_MOVE_X_INCREMENT}
        {MOUSE_MOVE_Y_DECREMENT}
        {MOUSE_MOVE_Y_INCREMENT}
        {MOUSE_WHEEL_UP}
        {MOUSE_WHEEL_DOWN}
        ```
        
   </details>

   - <details>
      
      <summary>Mapping a special character</summary>
        
        Special characters are supported
        
        ```
        SWITCH_DECREASE={KEY_LEFT_CTRL}
        
        {KEY_LEFT_CTRL}
        {KEY_LEFT_SHIFT}
        {KEY_LEFT_ALT}
        {KEY_LEFT_GUI}
        {KEY_RIGHT_CTRL}
        {KEY_RIGHT_SHIFT}
        {KEY_RIGHT_ALT}
        {KEY_RIGHT_GUI}
        {KEY_UP_ARROW}
        {KEY_DOWN_ARROW}
        {KEY_LEFT_ARROW}
        {KEY_RIGHT_ARROW}
        {KEY_BACKSPACE}
        {KEY_TAB}
        {KEY_RETURN}
        {KEY_ESC}
        {KEY_INSERT}
        {KEY_DELETE}
        {KEY_PAGE_UP}
        {KEY_PAGE_DOWN}
        {KEY_HOME}
        {KEY_END}
        {KEY_CAPS_LOCK}
        {KEY_F1}
        {KEY_F2}
        {KEY_F3}
        {KEY_F4}
        {KEY_F5}
        {KEY_F6}
        {KEY_F7}
        {KEY_F8}
        {KEY_F9}
        {KEY_F10}
        {KEY_F11}
        {KEY_F12}
        {KEY_PRINT_SCREEN}
        {KEY_SCROLL_LOCK}
        {KEY_PAUSE}
        {KEY_NUM_LOCK}
        {KEYPAD_DIVIDE}
        {KEYPAD_MULTIPLY}
        {KEYPAD_SUBTRACT}
        {KEYPAD_ADD}
        {KEYPAD_ENTER}
        {KEYPAD_1}
        {KEYPAD_2}
        {KEYPAD_3}
        {KEYPAD_4}
        {KEYPAD_5}
        {KEYPAD_6}
        {KEYPAD_7}
        {KEYPAD_8}
        {KEYPAD_9}
        {KEYPAD_0}
        {KEYPAD_DECIMAL}
        {KEY_APPLICATION}
        {KEY_MENU}
        ```
        
   </details>

   - Keydown and keyup events are mapped as the keyboard encounters them. So if you wanted to Ctrl+C. You would hold the CTRL button then tap C then release CTRL - the same as you would on any other keyboard. Complex macros combining strings and Special characters are not supported at this time.

3. **Update the Microcontroller Software**  
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

