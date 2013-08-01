#include <TimerOne.h>

boolean isMoving = 0;
boolean calibrationEnabled=true;
unsigned int timerTickCntr=0;
// all array indexes in clockwise order: TL, TR, BR, BL
byte desiredPowerPercent[4] = {0,0,0,0};
byte desiredPowerAbs[4] = {0,0,0,0};
byte realPowerAbs[4] = {0,0,0,0};
byte isDirectionForward[4] = {0,0,0,0};
byte encoderPrevState[4] = {0,0,0,0};

unsigned int currentSpeedAbs[4] = {0,0,0,0};

// total encoder ticks since start moving
unsigned long totalEncoderValue[4] = {0,0,0,0};
unsigned int calibrationEncoderValue[4] = {0,0,0,0};

void InitEncoders(){
  // set a timer of length 1000 microseconds (or 0.001 sec - or 1 kHz)
  currentSpeedAbs[0] =0;
  currentSpeedAbs[1] =0;
  currentSpeedAbs[2] =0;
  currentSpeedAbs[3] =0;
  calibrationEncoderValue[0] =0;
  calibrationEncoderValue[1] =0;
  calibrationEncoderValue[2] =0;
  calibrationEncoderValue[3] =0;
  timerTickCntr=0;
  Timer1.initialize(1000);
  Timer1.attachInterrupt(TimerInterruptHandler); // attach the service routine here
}


void DeactivateEncoders(){
  Timer1.detachInterrupt();
  Timer1.stop();
  //TODO:save calibrated vals to cache here
}

void TimerInterruptHandler() {
  if (encoderPrevState[0] != digitalRead(PI_MOTOR_ENC_TL)) {
    encoderPrevState[0]=!encoderPrevState[0];
    totalEncoderValue[0]++;
    calibrationEncoderValue[0]++;
  }
  if (encoderPrevState[1] != digitalRead(PI_MOTOR_ENC_TR)) {
    encoderPrevState[1]=!encoderPrevState[1];
    totalEncoderValue[1]++;
    calibrationEncoderValue[1]++;
  }
  if (encoderPrevState[2] != digitalRead(PI_MOTOR_ENC_BR)) {
    encoderPrevState[2]=!encoderPrevState[2];
    totalEncoderValue[2]++;
    calibrationEncoderValue[2]++;
  }
  if (encoderPrevState[3] != digitalRead(PI_MOTOR_ENC_BL)) {
    encoderPrevState[3]=!encoderPrevState[3];
    totalEncoderValue[3]++;
    calibrationEncoderValue[3]++;
  }
  
    if (timerTickCntr==200) {
      timerTickCntr=0;
      currentSpeedAbs[0] = calibrationEncoderValue[0];
      currentSpeedAbs[1] = calibrationEncoderValue[1];
      currentSpeedAbs[2] = calibrationEncoderValue[2];
      currentSpeedAbs[3] = calibrationEncoderValue[3];
      
      calibrationEncoderValue[0] = 0;
      calibrationEncoderValue[1] = 0;
      calibrationEncoderValue[2] = 0;
      calibrationEncoderValue[3] = 0;
      
      if (calibrationEnabled && currentSpeedAbs[0]>10 && currentSpeedAbs[1]>10 && currentSpeedAbs[2]>10 && currentSpeedAbs[3]>10)
        CalibrateMotors();
    }
    else 
      timerTickCntr++;
}

void MoveWheels(byte wheels, byte powerInPercent, boolean directionFwd=1) {
  DeactivateEncoders();
  //map percent to absolute byte value
  if (powerInPercent>100)
    powerInPercent=100;
  byte absPower;
  if(powerInPercent>0)  //TODO:get from cache here;
    absPower = map(powerInPercent, 1, 100, 0, 255);
  else 
    absPower = 0;   
  if (wheels & MOTOR_WHEEL_TL) {
    desiredPowerPercent[0] = powerInPercent;
    desiredPowerAbs[0]=absPower;
    isDirectionForward[0] = directionFwd;
    realPowerAbs[0] = absPower;
    digitalWrite(PO_MOTOR_DIR_TL,!directionFwd);
    analogWrite(PP_MOTOR_SPD_TL,realPowerAbs[0]);
  }
  if (wheels & MOTOR_WHEEL_TR) {
    desiredPowerPercent[1] = powerInPercent;
    desiredPowerAbs[1]=absPower;
    isDirectionForward[1] = directionFwd;
    realPowerAbs[1] = absPower;
    digitalWrite(PO_MOTOR_DIR_TR,!directionFwd);
    analogWrite(PP_MOTOR_SPD_TR,realPowerAbs[1]);
  }
  if (wheels & MOTOR_WHEEL_BR) {
    desiredPowerPercent[2] = powerInPercent;
    desiredPowerAbs[2]=absPower;
    isDirectionForward[2] = directionFwd;
    realPowerAbs[2] = absPower;
    digitalWrite(PO_MOTOR_DIR_BR,directionFwd);
    analogWrite(PP_MOTOR_SPD_BR,realPowerAbs[2]);
  }
  if (wheels & MOTOR_WHEEL_BL) {
    desiredPowerPercent[3] = powerInPercent;
    desiredPowerAbs[3]=absPower;
    isDirectionForward[3] = directionFwd;
    realPowerAbs[3] = absPower;
    digitalWrite(PO_MOTOR_DIR_BL,directionFwd);
    analogWrite(PP_MOTOR_SPD_BL,realPowerAbs[3]);
  }
  
  if (desiredPowerPercent[0] > 0 || desiredPowerPercent[1] > 0 || desiredPowerPercent[2] > 0 || desiredPowerPercent[3] > 0){
    isMoving = true;
    InitEncoders();
  }
  else {
    currentSpeedAbs[0] =0;
    currentSpeedAbs[1] =0;
    currentSpeedAbs[2] =0;
    currentSpeedAbs[3] =0;
    isMoving=false;
  }
}

void TurnLeft(byte powerPercent) {
  MoveWheels(MOTOR_WHEEL_TL | MOTOR_WHEEL_BL, powerPercent, false);
  MoveWheels(MOTOR_WHEEL_TR | MOTOR_WHEEL_BR, powerPercent, true);
}

void TurnRight(byte powerPercent) {
  MoveWheels(MOTOR_WHEEL_TL | MOTOR_WHEEL_BL, powerPercent, true);
  MoveWheels(MOTOR_WHEEL_TR | MOTOR_WHEEL_BR, powerPercent, false);
}

void MoveForward(byte powerPercent) {
  MoveWheels(MOTOR_WHEEL_TL | MOTOR_WHEEL_TR | MOTOR_WHEEL_BL | MOTOR_WHEEL_BR, powerPercent, true);
}

void MoveBackward(byte powerPercent) {
  MoveWheels(MOTOR_WHEEL_TL | MOTOR_WHEEL_TR | MOTOR_WHEEL_BL | MOTOR_WHEEL_BR, powerPercent, false);
}
  
void StopMoving() {
  MoveWheels(MOTOR_WHEEL_TL | MOTOR_WHEEL_TR | MOTOR_WHEEL_BL | MOTOR_WHEEL_BR, 0);
}

void CalibrateMotors(){
  //prevent timer to trigger interrupt again while calibrating
  Timer1.detachInterrupt();
  boolean changed=false;
  
  //sides
  changed |= SynchMotorPair(0, 3); // TL and BL
  changed |= SynchMotorPair(1, 2); // TR and BR
  //shafts
  changed |= SynchMotorPair(0, 1); // TL and TR
  changed |= SynchMotorPair(2, 3); // BR and BL
  //diagonals
  changed |= SynchMotorPair(0, 2); // TL and BR
  changed |= SynchMotorPair(1, 3); // TR and BL
  
  if (!changed){
    float avgReal = (float)(realPowerAbs[0] + realPowerAbs[1] + realPowerAbs[2] + realPowerAbs[3]) / 4;
    float avgDesired = (float)(desiredPowerAbs[0] + desiredPowerAbs[1] + desiredPowerAbs[2] + desiredPowerAbs[3]) / 4;
    if (avgReal-avgDesired>=1){
      realPowerAbs[0]--;
      realPowerAbs[1]--;
      realPowerAbs[2]--;
      realPowerAbs[3]--;
    }
    else if (avgReal - avgDesired <= -1 && realPowerAbs[0]<255 && realPowerAbs[1]<255 && realPowerAbs[2]<255 && realPowerAbs[3]<255 ){
      realPowerAbs[0]++;
      realPowerAbs[1]++;
      realPowerAbs[2]++;
      realPowerAbs[3]++;
    }                 
  }
  //set the new power values
  analogWrite(PP_MOTOR_SPD_TL,realPowerAbs[0]);
  analogWrite(PP_MOTOR_SPD_TR,realPowerAbs[1]);
  analogWrite(PP_MOTOR_SPD_BR,realPowerAbs[2]);
  analogWrite(PP_MOTOR_SPD_BL,realPowerAbs[3]);
  
  //set interrupt back
  Timer1.attachInterrupt(TimerInterruptHandler);
}

boolean SynchMotorPair(byte motorIndex1, byte motorIndex2){
  if (realPowerAbs[motorIndex1] == 0 || realPowerAbs[motorIndex2] == 0)
    return false;
  if (currentSpeedAbs[motorIndex2] == 0)
    currentSpeedAbs[motorIndex2] = 1;
  float ratioDiff = ((float)currentSpeedAbs[motorIndex1] / currentSpeedAbs[motorIndex2]) - ((float)desiredPowerAbs[motorIndex1] / desiredPowerAbs[motorIndex2]);
  char absDiff1 = realPowerAbs[motorIndex1] - desiredPowerAbs[motorIndex1];
  char absDiff2 = realPowerAbs[motorIndex2] - desiredPowerAbs[motorIndex2];
  boolean isSameSign = (absDiff1 ^ absDiff2) >= 0;
  //motorIndex1 speed compared to motorIndex2 speed is slower then desired
  if (ratioDiff < 0.01){
    // here we have two options - either speed up motorIndex1 or slow down motorIndex2.
    // we choose that change, which will keep overall real power closer to desired value
    // e. g. any motor will always try to keep its power as close to desired as possible.
    if (realPowerAbs[motorIndex1] < 255 && ((!isSameSign && absDiff1 <= absDiff2) || (isSameSign && absDiff1 >= absDiff2)))
      realPowerAbs[motorIndex1]++;
    else
      realPowerAbs[motorIndex2]--;
    return true;
  }
  //motorIndex1 speed compared to motorIndex2 speed is faster then desired
  else if (ratioDiff > 0.01){
    // same two options - either slow down motorIndex1 or speed up motorIndex2.
    if (realPowerAbs[motorIndex2] == 255 || (isSameSign && absDiff1 <= absDiff2) || (!isSameSign && absDiff1 >= absDiff2))
      realPowerAbs[motorIndex1]--;
    else
      realPowerAbs[motorIndex2]++;
    return true;
  }
  return false;
}

