# Update 0.26.2

- improved scrolling in the menu
- Added a key combination to access the menu, to avoid accidental entry (Rear Button + second central button of the knob)

# Upload Orbion FIRMWARE on MCU

- Download and Installa Arduino IDE https://www.arduino.cc/en/software
- launch Arduino IDE and open the .ino file in the Orbion_FW_vX.XX folder (the icon.h file is also essential, so include it in the folder where the .ino file)
- Add the .ZIP libraries present in Orbion_FW_vX.XX/Library (Sketch -> #include library -> add library from .zip)
- select the board to use (Tools -> Board -> Arduino AVR Board -> Arduino Micro)
- select the port of your arduino (Tool -> Port -> (Arduino Micro) )
- now you are ready to upload the Firmware (Sketch -> Upload)
- now enjoy :)

#### Download only one version of the .ino file, for example the latest
