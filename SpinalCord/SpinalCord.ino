//main file with entry point
#include "Pinout.h"
#include "Constants.h"
#include <LED.h>
#include <EEPROM.h>
#include <TimerOne.h>
#if defined(DEBUG)
  #include <MemoryFree.h> //for debug purposes. Read More http://playground.arduino.cc/Code/AvailableMemory
#endif
#include <Servo.h>

typedef void (*functionPtr)();

//current and previous robot mode (strategy)
byte mode; 
byte prevMode;
boolean isModeUpdated = false;

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
  InitAllOutputPins();
  statusLED1->on(); //indicate serup is running
  
  DBG_ONLY(Serial.begin(38400));
  DBG_ONLY(Serial.println("Debug mode"));
  
  InitModeAndModeButton();
  InitStrategyMethods();
  InitIRSensor();
  CenterHead();
  delay(200);
  //setup finished
  statusLED1->off();
}

void loop() {
  // put your main code here, to run repeatedly:
  while (isModeUpdated) {
    isModeUpdated=false;
    statusLED2->blink(100);
    strategyMethods[2][prevMode](); // finish any operations for prevMode here
    strategyMethods[0][mode](); // init new strategy according to the new mode value
  }
  strategyMethods[1][mode]();
  //DBG_ONLY(Serial.print("Batt voltage="));
  //DBG_ONLY(Serial.println(analogRead(PA_BATT_VOLTAGE)));
}

//sets pointers for strategies methods
void InitStrategyMethods() {
  //start methods
  strategyMethods[0][0] = StartStrategyBlinker;
  strategyMethods[0][1] = StartStrategyTest;//StartStrategyLightSeeker;
  //strategyMethods[0][2] = Dummy;
  
  //run methods
  strategyMethods[1][0] = RunStrategyBlinker;
  strategyMethods[1][1] = RunStrategyTest;//RunStrategyLightSeeker;
  //strategyMethods[1][2] = Dummy;
  
  //finish methods
  strategyMethods[2][0] = FinishStrategyBlinker;
  strategyMethods[2][1] = FinishStrategyTest;//FinishStrategyLightSeeker;
  //strategyMethods[2][2] = Dummy;
}

//loops through button-selectable modes. Triggered by button interrupt
unsigned long lastMillis = 0;
void ModeButtonInterruptHandler() {
  if (millis() - lastMillis > 300) {
    lastMillis=millis();
    if (mode+1 < MODES_MIN_BROWSABLE || mode+1 >= MODES_MAX)
      SetMode(MODES_MIN_BROWSABLE);
    else 
      SetMode(mode+1);
  }
}

// set last mode, attach button interrupt
void InitModeAndModeButton() {
  pinMode(PI_BUTTON_MODE, INPUT);
  attachInterrupt(0, ModeButtonInterruptHandler, FALLING);
  mode = EEPROM.read(MEMADDR_LASTMODE);
  prevMode = 0;
  isModeUpdated = true;
}

//set leds
void InitStatusLeds() {
  statusLED1 = new LED(PP_LED_STATUS_1);
  statusLED2 = new LED(PP_LED_STATUS_2);
  statusLED1->off();
  statusLED2->off();
}

//sets pinmode of all pins and writes initial values for outputs
void InitAllOutputPins(){
  pinMode(PO_SONICSENSOR_TRIGGER, OUTPUT);
  digitalWrite(PO_SONICSENSOR_TRIGGER,LOW);
  
  pinMode(PO_IRBUMPER_SWITCH,OUTPUT);
  digitalWrite(PO_IRBUMPER_SWITCH,HIGH);
  
  pinMode(PO_IRANALOG_SWITCH,OUTPUT);
  digitalWrite(PO_IRANALOG_SWITCH,LOW);
  
  pinMode(PP_MOTOR_SPD_TL,OUTPUT);
  pinMode(PP_MOTOR_SPD_TR,OUTPUT);
  pinMode(PP_MOTOR_SPD_BL,OUTPUT);
  pinMode(PP_MOTOR_SPD_BR,OUTPUT);
  digitalWrite(PP_MOTOR_SPD_TL,LOW);
  digitalWrite(PP_MOTOR_SPD_TR,LOW);
  digitalWrite(PP_MOTOR_SPD_BL,LOW);
  digitalWrite(PP_MOTOR_SPD_BR,LOW);
  
  pinMode(PO_MOTOR_DIR_TL,OUTPUT);
  pinMode(PO_MOTOR_DIR_TR,OUTPUT);
  pinMode(PO_MOTOR_DIR_BL,OUTPUT);
  pinMode(PO_MOTOR_DIR_BR,OUTPUT);
  digitalWrite(PO_MOTOR_DIR_TL,MOTOR_FWD_T);
  digitalWrite(PO_MOTOR_DIR_TR,MOTOR_FWD_T);
  digitalWrite(PO_MOTOR_DIR_BL,MOTOR_FWD_B);
  digitalWrite(PO_MOTOR_DIR_BR,MOTOR_FWD_B);
  
  pinMode(PI_MOTOR_ENC_TL,INPUT);
  pinMode(PI_MOTOR_ENC_TR,INPUT);
  pinMode(PI_MOTOR_ENC_BL,INPUT);
  pinMode(PI_MOTOR_ENC_BR,INPUT);
  
}

// tries set the mode and isModeUpdated flag
boolean SetMode(byte newMode) {
  if (newMode < MODES_MAX) {
    prevMode = mode;
    mode = newMode;
    isModeUpdated = true;
    EEPROM.write(MEMADDR_LASTMODE, mode);
    DBG_ONLY(Serial.print("Mode is set to "));
    DBG_ONLY(Serial.println(mode));
    return true;
  }
  return false;
}
