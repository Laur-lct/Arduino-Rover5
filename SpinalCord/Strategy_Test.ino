//this is test robot strategy to try new things.
int cntr=0;
void StartStrategyTest() {
  //set do all preparations here
  DBG_ONLY(Serial.println("Strategy Test start"));
  DBG_ONLY(delay(1000));

  //DBG_ONLY(calibrationEnabled=!calibrationEnabled);
  DBG_ONLY(Serial.print("Calibration enabled:\t"));
  DBG_ONLY(Serial.println(isCalibrationEnabled));
  cntr=0;
}

void RunStrategyTest() {
  if (cntr==0)
    TurnLeft(30);
  else if (cntr==16){
    StopMoving();
    DBG_ONLY(Serial.println());
    delay(500);
    TurnRight(30);  
  }
  delay(250);
  
  
  
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
  DBG_ONLY(Serial.println(realPowerAbs[3]));
  if (cntr<32)
    cntr++;
  else   
    SetMode(0);
  
}

void FinishStrategyTest() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strategy Test finish"));
  StopMoving();
}
