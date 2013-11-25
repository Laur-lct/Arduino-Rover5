//main file with entry point
#include "Pinout.h"
#include "Constants.h"
#include <LED.h>
#include <EEPROM.h>
#include <TimerOne.h>
#if defined(DEBUG)
  #include <MemoryFree.h> //for debug purposes. Read More http://playground.arduino.cc/Code/AvailableMemory
#endif
#include <SoftwareServo.h>
#include <DigitalWriteFast.h>
#include <MsTimer2.h>
//compass stuff
#include <ADXL345.h>
#include <HMC58X3.h>
#include <ITG3200.h>
#include <FreeIMU.h>
#include <Wire.h>

#include "_shared.h"

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
  DBG_ONLY(Serial.begin(38400));
  DEBUG_PRINTLN("Debug mode. Entered setup...");
  // put your setup code here, to run once:
  InitStatusLeds();
  InitAllPins();
  statusLED1->on(); //indicate serup is running

  InitModeAndModeButton();
  InitStrategyMethods();
  InitIRSensor();
  InitServiceInterrupt();
  CenterHead();
  //InitCompass();
  InitBluetooth();
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
  //bluetooth receive and send
  ProcessBluetooth();
}

//sets pointers for strategies methods
void InitStrategyMethods() {
  
  strategyMethods[0][MODE_FREEIMU_CALIB] = StartFreeimuCalib;
  strategyMethods[1][MODE_FREEIMU_CALIB] = RunFreeimuCalib;
  strategyMethods[2][MODE_FREEIMU_CALIB] = FinishFreeimuCalib;
  
  strategyMethods[0][MODE_BLINKER] = StartStrategyBlinker;
  strategyMethods[1][MODE_BLINKER] = RunStrategyBlinker;
  strategyMethods[2][MODE_BLINKER] = FinishStrategyBlinker;
  
  strategyMethods[0][MODE_TEST] = StartStrategyTest;
  strategyMethods[1][MODE_TEST] = RunStrategyTest;
  strategyMethods[2][MODE_TEST] = FinishStrategyTest;
  
  strategyMethods[0][MODE_LIGHTSEEKER] = StartStrategyLightSeeker;
  strategyMethods[1][MODE_LIGHTSEEKER] = RunStrategyLightSeeker;
  strategyMethods[2][MODE_LIGHTSEEKER] = FinishStrategyLightSeeker;
  
  strategyMethods[0][MODE_REMOTECONTROL] = StartStrategyRemote;
  strategyMethods[1][MODE_REMOTECONTROL] = RunStrategyRemote;
  strategyMethods[2][MODE_REMOTECONTROL] = FinishStrategyRemote;
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
void InitAllPins(){
  pinMode(PI_BUTTON_MODE, INPUT);
  
  pinMode(PO_SELF_POWEROFF, OUTPUT);
  digitalWrite(PO_SELF_POWEROFF,LOW);
  
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

void SelfPowerOff()
{
  digitalWrite(PO_SELF_POWEROFF,HIGH);
}

// tries set the mode and isModeUpdated flag
boolean SetMode(byte newMode) {
  if (newMode < MODES_MAX) {
    prevMode = mode;
    mode = newMode;
    isModeUpdated = true;
    EEPROM.write(MEMADDR_LASTMODE, mode);
    DEBUG_PRINT("Mode is set to ");
    DEBUG_PRINTLN(mode);
    return true;
  }
  return false;
}
