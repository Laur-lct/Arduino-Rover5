//this is test robot strategy to try new things.
int cntr=0;
byte bumperVal;
void StartStrategyTest() {
  //set do all preparations here
  DBG_ONLY(Serial.println("Strategy Test start"));
  DBG_ONLY(delay(1000));

  //DBG_ONLY(calibrationEnabled=!calibrationEnabled);
  DBG_ONLY(Serial.print("Calibration enabled:\t"));
  DBG_ONLY(Serial.println(isCalibrationEnabled));
  cntr=0;
  bumperVal=0;
}

void RunStrategyTest() {
  bumperVal=0;
  if (!digitalRead(PI_IRBUMPER_TL))
    bumperVal+=SENSOR_IRBUMP_TL;
  if (!digitalRead(PI_IRBUMPER_TR))
    bumperVal+=SENSOR_IRBUMP_TR;
  if (!digitalRead(PI_IRBUMPER_BR))
    bumperVal+=SENSOR_IRBUMP_BR;
  if (!digitalRead(PI_IRBUMPER_BL))
    bumperVal+=SENSOR_IRBUMP_BL;
  DBG_ONLY(Serial.print("Bumper :\t"));
  DBG_ONLY(Serial.println(bumperVal));
  
  if (bumperVal == 0)
    MoveForward(30);
  else if (bumperVal & SENSOR_IRBUMP_TL){
    MoveBackward(20);
    delay(800);
    TurnRight(30);
    delay(1000);
  }  
  else if (bumperVal & SENSOR_IRBUMP_TR){
    MoveBackward(20);
    delay(800);
    TurnLeft(30);
    delay(1000);
  }
  delay(250);
  /*
  DBG_ONLY(Serial.print("Speed:\t"));
  DBG_ONLY(Serial.print(currentSpeedAbs[0]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(currentSpeedAbs[1]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(currentSpeedAbs[2]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(currentSpeedAbs[3]));
  
  DBG_ONLY(Serial.print("\tReal:\t"));
  DBG_ONLY(Serial.print(realPowerAbs[0]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(realPowerAbs[1]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(realPowerAbs[2]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.println(realPowerAbs[3]));*/
  
  if (cntr<256)
    cntr++;
  else   
    SetMode(0);
  
}

void FinishStrategyTest() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strategy Test finish"));
  StopMoving();
}
