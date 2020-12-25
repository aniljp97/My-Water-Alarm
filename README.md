# My-Water-Alarm
Personal project of electronic that works as a basic alarm clock which makes you drink a glass of water in the morning when you wake up.

**Working, Usable, and Complete Compact Set Up**
This creation is a basic alarm clock attached to a scale that you set a glass/cup of water on to arm the alarm. Once the alarm is going off, it is stopped by placing the same glass/cup back onto the scale without any water in it. 

It features 2 buttons which can control 4 features: cycle brightness of the display, set alarm time, check if the alarm is armed by reading the weight on the scale, zero-out/tare the scale with the current weight on it. These features are accessed by taps and 2 second holds of the buttons. The display is, outside of access of features displaying, shows the current time using a Real Time Clock so even through loss of power, the time will never need to be set to the proper time. The alarm sound uses a single Piezo buzzer. The modules are controlled by a Arduino Uno.

# Purpose
Wanted to make an alarm that was not so easily snoozed and promotes a healthy habit of hydrating in the morning.

This wiki's main purpose to overview the make up of this projects code and hardware and to make use and understanding of this creation easier.

# Materials
(module text is also a link to online reference that was used for research, code, and help)
* Arduino Nano
* [Load Cell and HX711 Amplifier](https://www.instructables.com/Arduino-Scale-With-5kg-Load-Cell-and-HX711-Amplifi/)
* [TM1637 4-Digit 7-Segment Display](https://www.makerguides.com/tm1637-arduino-tutorial/)
* [DS1307 RTC Module](https://lastminuteengineers.com/ds1307-rtc-arduino-tutorial/)
* [Piezo Buzzer and 2 Tactical Switch Buttons](https://learn.sparkfun.com/tutorials/sik-experiment-guide-for-arduino---v33/introduction-hardware)
* Female-to-female and male-to-female wires
* 2 10k resistors
* 30mm x 70mm PCB board
* Small Breadboard (optional but used to make multiple connections to VCC and GND possible)
* Mini-USB connector
* USB charging block
 
# Wiring
![](https://github.com/aniljp97/My-Water-Alarm/blob/main/Images/wiring%20table.png)

# Container
Cheap plastic box with lid about 4" x 3" x 1.5" dimensions.  

A 0.5 diameter hole is drilled into each side. Out of the right side hole the wires of the digit display, the piezo buzzer, and the scale come through. 
![](https://github.com/aniljp97/My-Water-Alarm/blob/main/Images/box%20rightside%20hole.jpg)

The left side hole has the power/Arduino Nano cable coming through it.

The lid has an about 0.75" x 1.5" rectangle in the middle of it cut using a box cutter.  This will feed the 2 buttons connected to a small PCB board which have male wires soldered to it on the back side. The PCB board is connected to the lid with 2 sided tape. 
![](https://github.com/aniljp97/My-Water-Alarm/blob/main/Images/box%20top%20hole.jpg)

# Complete Setup
You can see the use of the buttons in the sticky notes.
![](https://github.com/aniljp97/My-Water-Alarm/blob/main/Images/complete%20set%20up.jpg)

# Code
All code is well commented and methods are explained. You can see the source code [here](https://github.com/aniljp97/My-Water-Alarm/blob/main/Source%20Code/MY_water_alarm.ino).
### Setup

![](https://github.com/aniljp97/My-Water-Alarm/blob/main/Images/setup%20code%20SC.png)

### Loop
![](https://github.com/aniljp97/My-Water-Alarm/blob/main/Images/loop%20code%20SC.png)

# Notes for Use
If the product loses power, when it is turned back on the setup() code is run again. In this possible situation, suggestions are to leave empty cup/glass on the scale at all times because the scale is zeroed on power up. Also the set alarm time will go to default if powered back up so setting the global variables ALARM_HOUR and ALARM_MINUTE to what you want as default time.

Use the right side button with a tap to check if the alarm is set if you are not sure.  Make sure the hold the right side button for 2 seconds to zero-out the scale with the empty glass/cup that you are using to hold the water.

When is set alarm time mode by holding the left-side button for 2 seconds, you are changing the hours and then the minute. Whatever digit you are changing will be solid and all other digits will be blinking. Confirm a digit by tapping the right-side button and select the number with the left-side button. When confirming the minutes, the full set time should blink 3 times before going back to the current time.

NOTE: the current real time displays in 12-hour format, but when setting the alarm time, your selecting in 24-hour format. This is because the lack of being able to distinguish between pm and am. 

# Conclusion
### Project State
Works great and perfect so far. Fully usable and no maintenance needed. Very compact. 

### Next Steps
Could be made more portable by having less total wiring since a breadboard was used. Going to keep note of long-term stability and continue testing.

# Problems and Code Changes To Fix Problems
The alarm would go off for 8 seconds and stop by itself even when the alarm status is OFF. This is due to the lack of the conditional in the loop() function to check if the alarm is armed while also checking if the alarm's set time matches the current real time. [Here](https://github.com/aniljp97/My-Water-Alarm/commit/daa87638e19459480f94d3012e5a3c2624a8a813#diff-8e1d0b47b8c14d332ec54a50a5d41df5) are the few code changes made including the adding of a global variable and other code changes made accordingly.

Alarm consistently used every weekday for a month and half. No problems or things to be updated found yet.
