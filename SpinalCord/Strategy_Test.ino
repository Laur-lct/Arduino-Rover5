//this is test robot strategy to try new things.
int cntr=0;
void StartStrategyTest() {
  //set do all preparations here
  DBG_ONLY(Serial.println("Strategy Test start"));
  DBG_ONLY(delay(1000));

  DBG_ONLY(calibrationEnabled=!calibrationEnabled);
  DBG_ONLY(Serial.print("Calibration enabled:\t"));
  DBG_ONLY(Serial.println(calibrationEnabled));
  cntr=0;
}

void RunStrategyTest() {
  if (!isMoving) {
    MoveForward(50);
    DBG_ONLY(Serial.print("Desired:\t"));
    DBG_ONLY(Serial.print(desiredPowerAbs[0]));
    DBG_ONLY(Serial.print('\t'));
    DBG_ONLY(Serial.print(desiredPowerAbs[1]));
    DBG_ONLY(Serial.print('\t'));
    DBG_ONLY(Serial.print(desiredPowerAbs[2]));
    DBG_ONLY(Serial.print('\t'));
    DBG_ONLY(Serial.println(desiredPowerAbs[3]));
  }
  delay(200);
  
  DBG_ONLY(Serial.print("Speed:\t"));
  DBG_ONLY(Serial.print(currentSpeedAbs[0]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(currentSpeedAbs[1]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(currentSpeedAbs[2]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(currentSpeedAbs[3]));
  
  DBG_ONLY(Serial.print('\t'));
  
  DBG_ONLY(Serial.print("Real:\t"));
  DBG_ONLY(Serial.print(realPowerAbs[0]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(realPowerAbs[1]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(realPowerAbs[2]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.println(realPowerAbs[3]));
 
 cntr++;
if (cntr>=50)
  SetMode(0);
}

void FinishStrategyTest() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strategy Test finish"));
  StopMoving();
  DBG_ONLY(Serial.print("Desired:\t"));
    DBG_ONLY(Serial.print(desiredPowerAbs[0]));
    DBG_ONLY(Serial.print('\t'));
    DBG_ONLY(Serial.print(desiredPowerAbs[1]));
    DBG_ONLY(Serial.print('\t'));
    DBG_ONLY(Serial.print(desiredPowerAbs[2]));
    DBG_ONLY(Serial.print('\t'));
    DBG_ONLY(Serial.println(desiredPowerAbs[3]));
}
