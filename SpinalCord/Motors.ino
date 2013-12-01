boolean isMoving = false;
boolean isCalibrationEnabled=true;
boolean isCurrentMeasurementEnabled=false;
int timerTickCntr=0;
// all array indexes in clockwise order: TL, TR, BR, BL
byte desiredPowerPercent[4] = {0,0,0,0};
byte desiredPowerAbs[4] = {0,0,0,0};
byte realPowerAbs[4] = {0,0,0,0};
boolean isDirectionForward[4] = {0,0,0,0};
byte encoderPrevState[4] = {0,0,0,0};
int currentDraw[4] = {0,0,0,0};
unsigned int currentSpeedAbs[4] = {0,0,0,0};

// total encoder ticks since start moving
unsigned long totalEncoderValue[4] = {0,0,0,0};
unsigned int calibrationEncoderValue[4] = {0,0,0,0};
byte lastDirectionsByte;
unsigned long stopAtEncoderValue=0;

void InitEncoders(){
  totalEncoderValue[0] =0;
  totalEncoderValue[1] =0;
  totalEncoderValue[2] =0;
  totalEncoderValue[3] =0;
  stopAtEncoderValue=0;
  calibrationEncoderValue[0] =0;
  calibrationEncoderValue[1] =0;
  calibrationEncoderValue[2] =0;
  calibrationEncoderValue[3] =0;
  currentDraw[0]=0;
  currentDraw[1]=0;
  currentDraw[2]=0;
  currentDraw[3]=0;
  timerTickCntr=-200; // no sense to run motor calibration from the very start
  Timer1.initialize(MOTOR_TIMER_INTERVAL);
  Timer1.attachInterrupt(TimerInterruptHandler); // attach the service routine here
}

void DeactivateEncoders(){
  Timer1.detachInterrupt();
  Timer1.stop();
}

void TimerInterruptHandler() {
  if (encoderPrevState[0] != digitalReadFast(PI_MOTOR_ENC_TL)) {
    encoderPrevState[0]=!encoderPrevState[0];
    totalEncoderValue[0]++;
    calibrationEncoderValue[0]++;
  }
  if (encoderPrevState[1] != digitalReadFast(PI_MOTOR_ENC_TR)) {
    encoderPrevState[1]=!encoderPrevState[1];
    totalEncoderValue[1]++;
    calibrationEncoderValue[1]++;
  }
  if (encoderPrevState[2] != digitalReadFast(PI_MOTOR_ENC_BR)) {
    encoderPrevState[2]=!encoderPrevState[2];
    totalEncoderValue[2]++;
    calibrationEncoderValue[2]++;
  }
  if (encoderPrevState[3] != digitalReadFast(PI_MOTOR_ENC_BL)) {
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
    
    if (isCalibrationEnabled)
      CalibrateMotors();
  }
  else {
    timerTickCntr++;
    //if enabled, take motor current draw measurements. Each analogRead takes about 100 microseconds, so take measurements part by part.
    if (isCurrentMeasurementEnabled){
      if (timerTickCntr == MOTOR_CALIBRATION_TICK/4)
        currentDraw[0] = (analogRead(PA_MOTOR_CURRENT_TL) + analogRead(PA_MOTOR_CURRENT_TL))/2;
      else if (timerTickCntr == MOTOR_CALIBRATION_TICK/4 + 2)
        currentDraw[1] = (analogRead(PA_MOTOR_CURRENT_TR) + analogRead(PA_MOTOR_CURRENT_TR))/2;
      else if (timerTickCntr == MOTOR_CALIBRATION_TICK/4 + 4)
        currentDraw[2] = (analogRead(PA_MOTOR_CURRENT_BR) + analogRead(PA_MOTOR_CURRENT_BR))/2;
      else if (timerTickCntr == MOTOR_CALIBRATION_TICK/4 + 6)
        currentDraw[3] = (analogRead(PA_MOTOR_CURRENT_BL) + analogRead(PA_MOTOR_CURRENT_BL))/2;
    }
  }
  if (stopAtEncoderValue>0){
    long diff =  stopAtEncoderValue*4 - (totalEncoderValue[0]+totalEncoderValue[1]+totalEncoderValue[2]+totalEncoderValue[3]);
    if (diff<5 || timerTickCntr==-201)
      StopMoving();
    else if (diff < (40 + desiredPowerPercent[0]/10) && timerTickCntr>0){
      timerTickCntr = -900; //prevent calibration while braking
      analogWrite(PP_MOTOR_SPD_TL,25);
      analogWrite(PP_MOTOR_SPD_TR,25);
      analogWrite(PP_MOTOR_SPD_BR,25);
      analogWrite(PP_MOTOR_SPD_BL,25);
    }
  }
}

void MoveWheels(byte wheelDirections, byte powerPercentTL, byte powerPercentTR, byte powerPercentBR, byte powerPercentBL) {
  // do nothing if arguments are the same
  if (powerPercentTL == desiredPowerPercent[0] && 
      powerPercentTR == desiredPowerPercent[1] && 
      powerPercentBR == desiredPowerPercent[2] && 
      powerPercentBL == desiredPowerPercent[3] && 
      lastDirectionsByte == wheelDirections)
      return;
 
  if (isMoving ){
    DeactivateEncoders();
    if (isCalibrationEnabled)
      SaveRealToCache();
  }
  lastDirectionsByte = wheelDirections;
  // preset arrays 
  desiredPowerPercent[0] = powerPercentTL > 100 ? 100 : powerPercentTL;
  desiredPowerAbs[0] = desiredPowerPercent[0] > 0 ? map(desiredPowerPercent[0], 1, 100, 0, 255) : 0;
  isDirectionForward[0] = (wheelDirections & BINARY_CODE_TL) > 0;

  desiredPowerPercent[1] = powerPercentTR > 100 ? 100 : powerPercentTR;
  desiredPowerAbs[1] = desiredPowerPercent[1] > 0 ? map(desiredPowerPercent[1], 1, 100, 0, 255) : 0;
  isDirectionForward[1] = (wheelDirections & BINARY_CODE_TR) > 0;
 
  desiredPowerPercent[2] = powerPercentBR > 100 ? 100 : powerPercentBR;
  desiredPowerAbs[2] = desiredPowerPercent[2] > 0 ? map(desiredPowerPercent[2], 1, 100, 0, 255) : 0;
  isDirectionForward[2] = (wheelDirections & BINARY_CODE_BR) > 0;

  desiredPowerPercent[3] = powerPercentBL > 100 ? 100 : powerPercentBL;
  desiredPowerAbs[3] = desiredPowerPercent[3] > 0 ? map(desiredPowerPercent[3], 1, 100, 0, 255) : 0;
  isDirectionForward[3] = (wheelDirections & BINARY_CODE_BL) > 0;
 
    //set the vals
  realPowerAbs[0] = isCalibrationEnabled ? MapRealFromCache(0) : desiredPowerAbs[0];
  digitalWriteFast(PO_MOTOR_DIR_TL,!isDirectionForward[0]);
  analogWrite(PP_MOTOR_SPD_TL,realPowerAbs[0]);

  realPowerAbs[1] = isCalibrationEnabled ? MapRealFromCache(1) : desiredPowerAbs[1];
  digitalWriteFast(PO_MOTOR_DIR_TR,!isDirectionForward[1]);
  analogWrite(PP_MOTOR_SPD_TR,realPowerAbs[1]);

  realPowerAbs[2] = isCalibrationEnabled ? MapRealFromCache(2) : desiredPowerAbs[2];
  digitalWriteFast(PO_MOTOR_DIR_BR,isDirectionForward[2]);
  analogWrite(PP_MOTOR_SPD_BR,realPowerAbs[2]);

  realPowerAbs[3] = isCalibrationEnabled ? MapRealFromCache(3) : desiredPowerAbs[3];
  digitalWriteFast(PO_MOTOR_DIR_BL,isDirectionForward[3]);
  analogWrite(PP_MOTOR_SPD_BL,realPowerAbs[3]);
  
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

void TurnLeft(byte powerPercent, unsigned int deltaDegrees, boolean delayWhileMoving) {
  MoveWheels(BINARY_CODE_TR | BINARY_CODE_BR, powerPercent, powerPercent, powerPercent, powerPercent);
  if (deltaDegrees){
    stopAtEncoderValue = totalEncoderValue[0] + (unsigned long)(5.35f * deltaDegrees); //ideally 3.8353  encoder ticks per degree
    while(delayWhileMoving && isMoving)
      delay(20);
  }
}

void TurnRight(byte powerPercent, unsigned int deltaDegrees, boolean delayWhileMoving) {
  MoveWheels(BINARY_CODE_TL | BINARY_CODE_BL, powerPercent, powerPercent, powerPercent, powerPercent);
  if (deltaDegrees){
    stopAtEncoderValue = totalEncoderValue[0] + (unsigned long)(5.35f * deltaDegrees); //ideally 3.8353 encoder ticks per degree
    while(delayWhileMoving && isMoving)
      delay(20);
  }
}

void MoveForward(byte powerPercent, unsigned int distanceCm, boolean delayWhileMoving) {
  MoveWheels(BINARY_CODE_TL | BINARY_CODE_TR | BINARY_CODE_BL | BINARY_CODE_BR, powerPercent, powerPercent, powerPercent, powerPercent);
  if (distanceCm){
    stopAtEncoderValue = totalEncoderValue[0] + (unsigned long)(16.32f * distanceCm); // encoder ticks per CM
    while(delayWhileMoving && isMoving)
      delay(20);
  }
}

void MoveBackward(byte powerPercent, unsigned int distanceCm, boolean delayWhileMoving) {
  MoveWheels(0, powerPercent, powerPercent, powerPercent, powerPercent);
  if (distanceCm){
    stopAtEncoderValue = totalEncoderValue[0] + (unsigned long)(16.32f * distanceCm); // encoder ticks per CM 
    while(delayWhileMoving && isMoving)
      delay(20);
  }
}
  
void StopMoving() {
  MoveWheels(BINARY_CODE_TL | BINARY_CODE_TR | BINARY_CODE_BL | BINARY_CODE_BR, 0,0,0,0);
}

void CalibrateMotors(){
  //prevent timer to trigger interrupt again while calibrating
  Timer1.detachInterrupt();
  //DEBUG_PRINT("Calibrating...");
  
  boolean changed=false;
  //sides
  changed |= SyncMotorPair(3, 0); // TL and BL
  changed |= SyncMotorPair(1, 2); // TR and BR
  //shafts
  changed |= SyncMotorPair(0, 1); // TL and TR
  changed |= SyncMotorPair(2, 3); // BR and BL
  //diagonals
  changed |= SyncMotorPair(0, 2); // TL and BR
  changed |= SyncMotorPair(1, 3); // TR and BL
  //DEBUG_PRINTLN();
  if (realPowerAbs[0]<desiredPowerAbs[0] && realPowerAbs[1]<desiredPowerAbs[1] && realPowerAbs[2]<desiredPowerAbs[2] && realPowerAbs[3]<desiredPowerAbs[3]){
    realPowerAbs[0]++;
    realPowerAbs[1]++;
    realPowerAbs[2]++;
    realPowerAbs[3]++;
  }
  else if (desiredPowerAbs[0] < 251 && realPowerAbs[0] > desiredPowerAbs[0] + 3 && 
           desiredPowerAbs[1] < 251 && realPowerAbs[1] > desiredPowerAbs[1] + 3 && 
           desiredPowerAbs[2] < 251 && realPowerAbs[2] > desiredPowerAbs[2] + 3 && 
           desiredPowerAbs[3] < 251 && realPowerAbs[3] > desiredPowerAbs[3] + 3) {
    realPowerAbs[0]--;
    realPowerAbs[1]--;
    realPowerAbs[2]--;
    realPowerAbs[3]--;                 
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
  if (currentSpeedAbs[motorIndex2] < 3){
    realPowerAbs[motorIndex2]+=min(5,255-realPowerAbs[motorIndex2]);
    return true;
  }
  if (currentSpeedAbs[motorIndex1] < 3){
    realPowerAbs[motorIndex1]+=min(5,255-realPowerAbs[motorIndex1]);;
    return true;
  }
  float ratioDiff = (float)currentSpeedAbs[motorIndex1] / (currentSpeedAbs[motorIndex1] + currentSpeedAbs[motorIndex2])
                  - (float)desiredPowerAbs[motorIndex1] / ((unsigned int)desiredPowerAbs[motorIndex1] + desiredPowerAbs[motorIndex2]);
  char absDiff1 = realPowerAbs[motorIndex1] - desiredPowerAbs[motorIndex1];
  char absDiff2 = realPowerAbs[motorIndex2] - desiredPowerAbs[motorIndex2];
  boolean isSameSign = (absDiff1 ^ absDiff2) >= 0;
  byte changeAmount = (abs(ratioDiff)>0.25 ? 4 : (abs(ratioDiff)>0.09 ? 2 : 1));
  //DEBUG_PRINT(ratioDiff);
  //DEBUG_PRINT('\t');
  //motorIndex1 speed compared to motorIndex2 speed is slower then desired
  if (ratioDiff < -0.01f){
    // here we have two options - either speed up motorIndex1 or slow down motorIndex2.
    // we choose that change, which will keep overall real power closer to desired value
    // e. g. any motor will always try to keep its power as close to desired as possible.
    if (realPowerAbs[motorIndex1] < 255 && ((!isSameSign && absDiff1 <= absDiff2) || (isSameSign && absDiff1 >= absDiff2)))
      realPowerAbs[motorIndex1]+=min(changeAmount,255-realPowerAbs[motorIndex1]);
    else
      realPowerAbs[motorIndex2]-=min(changeAmount,realPowerAbs[motorIndex2]);
    return true;
  }
  //motorIndex1 speed compared to motorIndex2 speed is faster then desired
  else if (ratioDiff > 0.01f){
    // same two options - either slow down motorIndex1 or speed up motorIndex2.
    if (realPowerAbs[motorIndex2] == 255 || (isSameSign && absDiff1 <= absDiff2) || (!isSameSign && absDiff1 >= absDiff2))
      realPowerAbs[motorIndex1]-=min(changeAmount,realPowerAbs[motorIndex1]);
    else
      realPowerAbs[motorIndex2]+=min(changeAmount,255-realPowerAbs[motorIndex2]);
    return true;
  }
  return false;
}

void DropMotorCache() {
  for (int i = MEMADDR_MOTORCACHE_START; i < MEMADDR_MOTORCACHE_END; i++) 
    EEPROM.write(i,0);
}

#if defined(DEBUG)
void PrintMotorCache(){
  for (int i = 0; i<MEMADDR_MOTORCACHE_END - MEMADDR_MOTORCACHE_START; i++){
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

