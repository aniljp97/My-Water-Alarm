#include "HX711.h"
#include <Wire.h>
#include "RTClib.h"
#include <TM1637Display.h>


// pins for HX711 weight scale module
#define HX711_DT_PIN 6 // HX711.DOUT	- pin D6
#define HX711_SCK_PIN 5 // HX711.PD_SCK	- pin D5

// pins for buttons
#define R_BUTTON_PIN 2 // Right Button - pin D2
#define L_BUTTON_PIN 4 // Left Button - pin D4

// pin for alarm buzzer
#define PIEZO_PIN 3 // Piezo Buzzer - pin D3

// pins for digit display
#define DISPLAY_CLK_PIN 10 // Display CLK - pin D10 
#define DISPLAY_DIO_PIN 11 // Display DIO - pin D11

// classes initialized for the modules
RTC_DS1307 RTC; //define DS1307 object for keeping real time defined by 'RTC'
HX711 scale(HX711_DT_PIN, HX711_SCK_PIN);		// weight scale defined by 'scale'
TM1637Display display = TM1637Display(DISPLAY_CLK_PIN, DISPLAY_DIO_PIN); // digit display defined by 'display'


void setup() {
  Serial.begin(38400); // Serial for program testing

  display.setBrightness(7); // initially setting displays brightness at the highest level
  
  // initialize buttons pins as input
  pinMode(R_BUTTON_PIN, INPUT);
  pinMode(L_BUTTON_PIN, INPUT);

  // initialize weight scale
  scale.set_scale(2280.f);    // this value is obtained by calibrating the scale with known weights
//  scale.set_offset(135582); // can be use to pre-set scale to a set weight
  scale.tare();				        // reset the scale to 0

  // define and set current time to RTC module
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
//    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

///////////////// Global variables used within loop() ///////////////////////
bool OFF_BUZZ_CHECK = false; // used to have the scale need to read 2 zero weights in a row to stop the alarm
bool BUZZ = false; // if true, indicates that the alarm should be going off

int SCALE_MARGIN = 5; // margin for weight inprecision for the scale

bool R_PUSHED = false; // if true, indicates that the right button is being pressed
bool L_PUSHED = false; // if true, indicates that the left button is being pressed
int BUTTON_PRESS_HOLD_TIME = 2000; // time (in millseconds) that it takes for a pressed button to count as a hold on the button
unsigned long PREV_TIME = 0; // keeps track of time in milliseconds of when an event happened.

int CURR_DISPLAY_BRIGHT = 7; // what the displays brightness is currently set to (0-7)

bool ALARM_ARMED = false;

int ALARM_HOUR = 6; // alarms set hour (24-hour format)
int ALARM_MINUTE = 15; // alarms set minute

unsigned long CURR_TIME = 0; // used to hold time in millis() to keep track of time since an events happen

// Segement activated arrays for digit display
const uint8_t on[] = {
  SEG_A | SEG_B | SEG_F | SEG_C | SEG_E | SEG_D,  // O
  SEG_A | SEG_B | SEG_F | SEG_C | SEG_E   // N
};
const uint8_t off[] = {
  SEG_A | SEG_B | SEG_F | SEG_C | SEG_E | SEG_D,  // O
  SEG_A | SEG_G | SEG_F | SEG_E,   // F
  SEG_A | SEG_G | SEG_F | SEG_E   // F
};
const uint8_t tare[] = {
  SEG_D | SEG_F | SEG_E | SEG_G,  // t
  SEG_A | SEG_G | SEG_F | SEG_B | SEG_E | SEG_C,   // A
  SEG_G | SEG_E,   // r
  SEG_A | SEG_F | SEG_G | SEG_E | SEG_D    // E
};
const uint8_t gulp[] = {
  SEG_A | SEG_F | SEG_E   | SEG_C | SEG_D,  // G
  SEG_D | SEG_F | SEG_B | SEG_E | SEG_C,   // U
  SEG_F | SEG_E | SEG_D,   // L
  SEG_A | SEG_F | SEG_G | SEG_E | SEG_B    // P
};
const uint8_t alar[] = {
  SEG_A | SEG_G | SEG_F | SEG_B | SEG_E | SEG_C,  // A
  SEG_F | SEG_E | SEG_D,   // L
  SEG_A | SEG_G | SEG_F | SEG_B | SEG_E | SEG_C,  // A
  SEG_G | SEG_E   // r
};

/////////////////////////////////////////////////////////////////////////////
// Summmary of loop() logic:
// Check if the the time is same as the set alarm. If so, alarm should be going off
// Check for right button being pushed. On tap, check if alarm is on. On hold, tare the scale.
// Check for left button being pushed. On tap, cycle the display brightness. On hold, set the alarm time.
// Check if the alarm should be going off. If so, set alarm off. Else display time.
void loop() {
  Serial.println(scale.get_units());
  // get current time from RTC
  DateTime now = RTC.now();
  int now_hour = now.hour(); 
  int now_minute = now.minute();
  // if it is the alarms set time and the alarm is armed, buzz the alarm
  if(now_hour == ALARM_HOUR && now_minute == ALARM_MINUTE && now.second() < 8 && ALARM_ARMED) { 
    BUZZ = true;
  }
  
  CURR_TIME = millis();  // Time in milliseconds from upload used to know time from an event

  // logic for right button:
  // if it is pressed, display that the alarm is on or off
  // if press held for more than # seconds, tare/calibrate the weight scale
  if(digitalRead(R_BUTTON_PIN) == LOW) { // If the button is being pushed
    if(R_PUSHED) { // if the button is being held
      if(CURR_TIME - PREV_TIME > BUTTON_PRESS_HOLD_TIME) { // if the button has been held for # seconds
        scale.tare(); // calibrate the scale to 0 with current weight
        display.setSegments(tare);
        delay(1500);
        R_PUSHED = false;
      }
    } else { 
      PREV_TIME = CURR_TIME;
      R_PUSHED = true;
    }
  } else if(R_PUSHED) { // the button has only been pressed 
    displayAlarmStatus();
    R_PUSHED = false;
  }

  // logic for left button:
  // if it is pressed, cycle the brightness of the display
  // if press held for more than # seconds, set the alarms time
  if(digitalRead(L_BUTTON_PIN) == LOW) {
    if(L_PUSHED) {
      if(CURR_TIME - PREV_TIME > BUTTON_PRESS_HOLD_TIME) {
        setAlarmsTime();
        L_PUSHED = false;
      }
    } else {
      PREV_TIME = CURR_TIME;
      L_PUSHED = true;
    }
  } else if(L_PUSHED) {
    changeDisplayBrightness();
    L_PUSHED = false;
  }

  
  if(BUZZ) { // if the alarm should be going off
    display.clear();
    tone(PIEZO_PIN, 1500, 600); // noise
    delay(600);
    display.setSegments(gulp); // false gulp on the display
    delay(500); // pause inbetween noises

    if(scale.get_units() > -1*SCALE_MARGIN && scale.get_units() < SCALE_MARGIN) {
      if(OFF_BUZZ_CHECK) { // causes the scale to need to read the weight as 0 twice before turning off
        BUZZ = false;
      } else {
        OFF_BUZZ_CHECK = true;
      }
    } else {
      OFF_BUZZ_CHECK = false;
    }
  } else { // display the time in 12-hour format
    displayCurrTime(now_hour, now_minute);
  }

  if(scale.get_units() > -1*SCALE_MARGIN) { // if the weight is at 0 or lighter the alarm is not set
    ALARM_ARMED = false;
  } else { // else the weight is higher than 0 and that means that the alarm should be ready to go off
    ALARM_ARMED = true;
  }
}



///////////////////////// Methods //////////////////////////

// Display time in hours:minutes in 12-hour format, hours are passed in as 24-hour format
void displayCurrTime(int now_hour, int now_minute) {
  // display clock to in 12-hour clock format (for now NOT worrying about denoting am or pm)
  if(now_hour <= 12) {
    if(now_hour == 0) {
      display.showNumberDecEx(1200 + now_minute, 0b01000000, false);
    } else {
      display.showNumberDecEx(now_hour*100 + now_minute, 0b01000000, false);
    }
  } else {
    display.showNumberDecEx((now_hour-12)*100 + now_minute, 0b01000000, false);
  }
}

// Display if the alarm is ready to go off or not, denoted by global boolean ALARM_ARMED
void displayAlarmStatus() {
  if(ALARM_ARMED) {
    display.clear();
    display.setSegments(on, 2, 2);
  } else {
    display.clear();
    display.setSegments(off, 3, 1);
  }
  delay(2000);
}


// Set the alarms time using the left button to increment the hours and minutes, and the
// right button the confirm those selected hours and minutes.
// The colon and the opposite side of digits on the display will blink to denote that its setting the alarm time.
// Setting hours in 24-hour format, and the method will not exit until both hours and minutes are confirmed
void setAlarmsTime() {
  display.setSegments(alar);
  delay(1500);
  display.clear();
  bool l_button_pressed = false;
  bool r_button_pressed = false;
  bool colon_off = true;
  unsigned long prev_time = 0;

  int curr_hour = ALARM_HOUR;
  int curr_minute = ALARM_MINUTE;
  
  // set hours
  while(true) {
    CURR_TIME = millis();
    if(CURR_TIME - prev_time >= 500) { // blinking colon and the minutes digits on and off every 500 milliseconds 
      if(colon_off) { 
        display.clear();
        display.showNumberDec(curr_hour, true, 2, 0);
        colon_off = false;
      } else {
        display.showNumberDecEx(curr_hour * 100 + curr_minute, 0b01000000, true);
        colon_off = true;
      }
      prev_time = CURR_TIME;
    }

    if(digitalRead(L_BUTTON_PIN) == LOW) { 
      l_button_pressed = true;
    } else if(l_button_pressed) { // on a press increment hours once, no matter how long is held
      ++curr_hour;
      // cycle through possible hours
      if(curr_hour == 24) {  
        curr_hour = 0;
      }
      display.showNumberDec(curr_hour * 100 + curr_minute, true);
      l_button_pressed = false;
    }

    if(digitalRead(R_BUTTON_PIN) == LOW) { // on a press, confirm the hours
      r_button_pressed = true;
    } else if(r_button_pressed) {
      r_button_pressed = false;
      break;
    }
  }

  delay(1000); // delay to avoid accidental double right button click
  
  // set minutes
  while(true) {
    CURR_TIME = millis();
    if(CURR_TIME - prev_time >= 500) { // blinking colon and the hour digits on and off every 500 milliseconds
      if(colon_off) { 
        display.clear();
        display.showNumberDec(curr_minute, true, 2, 2);
        colon_off = false;
      } else {
        display.showNumberDecEx(curr_hour * 100 + curr_minute, 0b01000000, true);
        colon_off = true;
      }
      prev_time = CURR_TIME;
    }

    if(digitalRead(L_BUTTON_PIN) == LOW) {
      l_button_pressed = true;
    } else if(l_button_pressed) { // on a press increment minutes by 5 once, no matter how long is held
      curr_minute += 5;
      // cycle through possible minutes
      if(curr_minute == 60) {
        curr_minute = 0;
      }
      display.showNumberDec(curr_hour * 100 + curr_minute, true, 4, 0);
      l_button_pressed = false;
    }
    

    if(digitalRead(R_BUTTON_PIN) == LOW) { // on a press, confirm the minutes
      r_button_pressed = true;
    } else if(r_button_pressed) {
      r_button_pressed = false;
      break;
    }
  }
  
  // blink the set alarm time 3 times
  display.clear();
  delay(500);
  display.showNumberDecEx(curr_hour * 100 + curr_minute, 0b01000000, false);
  delay(500);
  display.clear();
  delay(500);
  display.showNumberDecEx(curr_hour * 100 + curr_minute, 0b01000000, false);
  delay(500);
  display.clear();
  delay(500);
  display.showNumberDecEx(curr_hour * 100 + curr_minute, 0b01000000, false);
  delay(750);

  // set the selected alarm time to the global variables
  ALARM_HOUR = curr_hour;
  ALARM_MINUTE = curr_minute;
}


// Changes the display brightness which can be set to 0-7
void changeDisplayBrightness() {
  // cycles through brightness as 0 -> 1 -> 2 -> 3 -> 7
  switch(CURR_DISPLAY_BRIGHT) {
    case 7:
      CURR_DISPLAY_BRIGHT = 0;
      break;
    case 3:
      CURR_DISPLAY_BRIGHT = 7;
      break;
    default:
      CURR_DISPLAY_BRIGHT++;
  }
  display.setBrightness(CURR_DISPLAY_BRIGHT);
}
////////////////////////////////////////////////////////////
