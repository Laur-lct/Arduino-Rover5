//main file with entry point
#include "Pinout.h"
#include "Constants.h"
#include <Bounce.h>
#include <LED.h>

typedef void (*functionPtr)();

//current and previous robot mode (strategy)
byte mode; 
byte prevMode;
boolean isModeUpdated = false;

// debounce obj ptr Read more: http://playground.arduino.cc/code/bounce
Bounce *buttonDebouncer;

// LED wrapper ptrs Read more: http://playground.arduino.cc/Code/LED
LED *statusLED1;
LED *statusLED2;

// pointers to key strategy methods.
// [0][MODES_MAX] - start methods
// [1][MODES_MAX] - main run methods
// [2][MODES_MAX] - finish methods
functionPtr strategyMethods[3][MODES_MAX];


void setup() {
  // put your setup code here, to run once:
  InitStatusLeds();
  //indicate serup is running
  statusLED1->on();
  InitModeAndModeButton();
  InitStrategyMethods();
  
  //simulate hard work
  delay(1000);
  
  //setup finished
  statusLED1->off();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (CheckMode()) {
    isModeUpdated = false;
    do {
      strategyMethods[2][prevMode](); // finish any operations for prevMode here
      strategyMethods[0][mode](); // init new strategy according to the new mode value
    } while (isModeUpdated);
  }
  strategyMethods[1][mode]();
}

void InitStrategyMethods() {
  // strategy methods here
  strategyMethods[0][0] = StartStrategyBlinker;
  strategyMethods[0][1] = 0;
  strategyMethods[0][2] = 0;
  
  strategyMethods[1][0] = RunStrategyBlinker;
  strategyMethods[1][1] = 0;
  strategyMethods[1][2] = 0;
  
  strategyMethods[2][0] = FinishStrategyBlinker;
  strategyMethods[2][1] = 0;
  strategyMethods[2][2] = 0;
}

void InitModeAndModeButton() {
  pinMode(PI_BUTTON_MODE, INPUT);
  buttonDebouncer = &Bounce(PI_BUTTON_MODE, 5);
  //TODO: reload last from memory
  mode = 0;
  prevMode = mode;
}

void InitStatusLeds() {
  pinMode(PP_LED_STATUS_1, OUTPUT);
  pinMode(PP_LED_STATUS_2, OUTPUT);
  digitalWrite(PP_LED_STATUS_1,0);
  digitalWrite(PP_LED_STATUS_2,0);
  statusLED1 = &LED(PP_LED_STATUS_1);
  statusLED2 = &LED(PP_LED_STATUS_2);
}

//checks and changes mode value
boolean CheckMode() {
  if (buttonDebouncer->update()) {
    if(buttonDebouncer->read() == HIGH) {
      if (!SetMode(mode+1))
        SetMode(0);
      buttonDebouncer->rebounce(500);
      //blocks for 100 miliseconds!!
      statusLED2->blink(100);
    }
  }
  return isModeUpdated;
}

// tries set the mode and isModeUpdated flag
boolean SetMode(byte newMode) {
  if (newMode < MODES_MAX) {
    prevMode = mode;
    mode = newMode;
    isModeUpdated = true;
    //TODO: save last mode to memory
    return true;
  }
  return false;
}

