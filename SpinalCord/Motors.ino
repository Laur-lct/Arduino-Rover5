#include <TimerOne.h>

boolean isMoving = false;
boolean isCalibrationEnabled=true;
boolean speedWasAbove10 = false;
unsigned int timerTickCntr=0;
// all array indexes in clockwise order: TL, TR, BR, BL
byte desiredPowerPercent[4] = {0,0,0,0};
byte desiredPowerAbs[4] = {0,0,0,0};
byte realPowerAbs[4] = {0,0,0,0};
boolean isDirectionForward[4] = {0,0,0,0};
byte encoderPrevState[4] = {0,0,0,0};

unsigned int currentSpeedAbs[4] = {0,0,0,0};

// total encoder ticks since start moving
unsigned long totalEncoderValue[4] = {0,0,0,0};
unsigned int calibrationEncoderValue[4] = {0,0,0,0};

void InitEncoders(){
  currentSpeedAbs[0] =0;
  currentSpeedAbs[1] =0;
  currentSpeedAbs[2] =0;
  currentSpeedAbs[3] =0;
  calibrationEncoderValue[0] =0;
  calibrationEncoderValue[1] =0;
  calibrationEncoderValue[2] =0;
  calibrationEncoderValue[3] =0;
  timerTickCntr=0;
  Timer1.initialize(MOTOR_TIMER_INTERVAL);
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
  
    if (timerTickCntr >= MOTOR_CALIBRATION_TICK) {
      timerTickCntr=0;
      currentSpeedAbs[0] = calibrationEncoderValue[0];
      currentSpeedAbs[1] = calibrationEncoderValue[1];
      currentSpeedAbs[2] = calibrationEncoderValue[2];
      currentSpeedAbs[3] = calibrationEncoderValue[3];
      
      calibrationEncoderValue[0] = 0;
      calibrationEncoderValue[1] = 0;
      calibrationEncoderValue[2] = 0;
      calibrationEncoderValue[3] = 0;
      
      speedWasAbove10 = currentSpeedAbs[0]>10 && currentSpeedAbs[1]>10 && currentSpeedAbs[2]>10 && currentSpeedAbs[3]>10;
      if (isCalibrationEnabled && speedWasAbove10)
        CalibrateMotors();
    }
    else 
      timerTickCntr++;
}

void MoveWheels(byte wheels, byte powerInPercent, boolean directionFwd=1) {
  if (isMoving ){
    DeactivateEncoders();
    if (isCalibrationEnabled && speedWasAbove10)
      SaveRealToCache();
  }
  //map percent to absolute byte value
  if (powerInPercent>100) 
    powerInPercent=100;
  byte absPower = powerInPercent > 0 ? map(powerInPercent, 1, 100, 0, 255) : 0;
  // preset arrays 
  if (wheels & MOTOR_WHEEL_TL) {
    desiredPowerPercent[0] = powerInPercent;
    desiredPowerAbs[0]=absPower;
    isDirectionForward[0] = directionFwd;
  }
  if (wheels & MOTOR_WHEEL_TR) {
    desiredPowerPercent[1] = powerInPercent;
    desiredPowerAbs[1]=absPower;
    isDirectionForward[1] = directionFwd;
  }
  if (wheels & MOTOR_WHEEL_BR) {
    desiredPowerPercent[2] = powerInPercent;
    desiredPowerAbs[2]=absPower;
    isDirectionForward[2] = directionFwd;
  }
  if (wheels & MOTOR_WHEEL_BL) {
    desiredPowerPercent[3] = powerInPercent;
    desiredPowerAbs[3]=absPower;
    isDirectionForward[3] = directionFwd;
  }
  
  //set the vals
  if (wheels & MOTOR_WHEEL_TL) {
    realPowerAbs[0] = isCalibrationEnabled ? MapRealFromCache(0) : desiredPowerAbs[0];
    digitalWrite(PO_MOTOR_DIR_TL,!directionFwd);
    analogWrite(PP_MOTOR_SPD_TL,realPowerAbs[0]);
  }
  if (wheels & MOTOR_WHEEL_TR) {
    realPowerAbs[1] = isCalibrationEnabled ? MapRealFromCache(1) : desiredPowerAbs[1];
    digitalWrite(PO_MOTOR_DIR_TR,!directionFwd);
    analogWrite(PP_MOTOR_SPD_TR,realPowerAbs[1]);
  }
  if (wheels & MOTOR_WHEEL_BR) {
    realPowerAbs[2] = isCalibrationEnabled ? MapRealFromCache(2) : desiredPowerAbs[2];
    digitalWrite(PO_MOTOR_DIR_BR,directionFwd);
    analogWrite(PP_MOTOR_SPD_BR,realPowerAbs[2]);
  }  
  if (wheels & MOTOR_WHEEL_BL) {
    realPowerAbs[3] = isCalibrationEnabled ? MapRealFromCache(3) : desiredPowerAbs[3];
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
  //DBG_ONLY(Serial.println("Calibrating..."));
  boolean changed=false;
  
  //sides
  changed |= SyncMotorPair(0, 3); // TL and BL
  changed |= SyncMotorPair(1, 2); // TR and BR
  //shafts
  changed |= SyncMotorPair(0, 1); // TL and TR
  changed |= SyncMotorPair(2, 3); // BR and BL
  //diagonals
  changed |= SyncMotorPair(0, 2); // TL and BR
  changed |= SyncMotorPair(1, 3); // TR and BL
  
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

boolean SyncMotorPair(byte motorIndex1, byte motorIndex2){
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

void DropCalibrationCache() {
  for (int i = MEMADDR_MOTORCACHE_START; i < MEMADDR_MOTORCACHE_END; i++) 
    EEPROM.write(i,0);
}

#if defined(DEBUG)
void PrintAllCache(){
  for (int i = 0; i<160; i++){
    if (i%8==0) {
      Serial.println();
      Serial.print("Iter ");
      Serial.print(i/8);
      Serial.print("\tVals:\t");
    }
    Serial.print(EEPROM.read(MEMADDR_MOTORCACHE_START+i));
    Serial.print("\t");
  }
  Serial.println();
}
#endif

void SaveRealToCache() {
  int memOffset;
  
  //DBG_ONLY(Serial.println("Saving to cache"));
  for (byte i=0; i < 4; i++){
    memOffset = GetCacheOffset(i);
    if (memOffset!=-1) {
      EEPROM.write(MEMADDR_MOTORCACHE_START + memOffset,realPowerAbs[i]);
      EEPROM.write(MEMADDR_MOTORCACHE_START + memOffset + 1, desiredPowerAbs[i]);
    }
  }
}

//tries to get cached real motor value. If not available, returns desirePowerdAbs for that motor.
byte MapRealFromCache (byte motorIndex) {
  if (desiredPowerAbs[motorIndex]==0)
    return 0;

  //DBG_ONLY(Serial.println("Reading from cache"));
  int memOffset = GetCacheOffset(motorIndex);
  if (memOffset < 0)
    return desiredPowerAbs[motorIndex];
    
  byte val = EEPROM.read(MEMADDR_MOTORCACHE_START + memOffset);
  //cache not set
  if (val==0)
    return desiredPowerAbs[motorIndex];
  return (byte)(((float)val/EEPROM.read(MEMADDR_MOTORCACHE_START + memOffset + 1)) * desiredPowerAbs[motorIndex] +0.5f);
}

// gets cached data relative memory offset. Negative number if offset not possible.
int GetCacheOffset(byte motorIndex) {
    // cache data formatted as 4-dim array: [dirVariant][desiredAbsGroup][motorIndex][valType]:
    // [dirVariant] from 0 to 3 , where 0 - all motors forward; 1 - all backward; 2 - left backward and right forward; 3 - left forward and right backward
    // [desiredPercentGroup] - from 0 to 4, where 0 - desiredPowerPercent between 1 and 20; 1 - between 21 and 40; 2 - between 41 and 60; 2 - between 61 and 80; 4 - between 81 and 100;
    // [motorIndex] - from 0 to 3. Simply motor index in array
    // [valType] - 0 to 1 : 0 - realAbsValue after calibration; 1 - absDesiredValue;
  int memOffset = motorIndex * 2;
  // all fwd
  if (isDirectionForward[0] == true && isDirectionForward[1] == true && isDirectionForward[2] == true && isDirectionForward[3] == true)
    memOffset+=0*5*4*2;
  //all back
  else if (isDirectionForward[0] == false && isDirectionForward[1] == false && isDirectionForward[2] == false && isDirectionForward[3] == false)
    memOffset+=1*5*4*2;
  // left backw, right fwd
  else if (((motorIndex==0 || motorIndex==3) && isDirectionForward[motorIndex] == false && isDirectionForward[ motorIndex==0 ? 1 : 2] == true)
           || ((motorIndex==1 || motorIndex==2) && isDirectionForward[motorIndex] == true &&  isDirectionForward[ motorIndex==1 ? 0 : 3] == false))
    memOffset+=2*5*4*2;
  // left fwd, right backw
  else if (((motorIndex==0 || motorIndex==3) && isDirectionForward[motorIndex] == true &&  isDirectionForward[ motorIndex==0 ? 1 : 2] == false)
           || ((motorIndex==1 || motorIndex==2) && isDirectionForward[motorIndex] == false &&  isDirectionForward[ motorIndex==1 ? 0 : 3] == true))
    memOffset+=3*5*4*2;  
  else 
    return -1;
  // add [desiredPercentGroup] offset
  memOffset += (int)((float)(desiredPowerPercent[motorIndex]-11)/20 + 0.5) * 4 * 2;

  //DBG_ONLY(Serial.print("Offset = \t"));
  //DBG_ONLY(Serial.print(memOffset));
  //DBG_ONLY(Serial.print("\tmotorIdx =\t"));
  //DBG_ONLY(Serial.println(motorIndex));
  return memOffset;
}

