//this is test robot strategy to try new things.
int cntr=0;
int cmLeft=100;
int cmRight=100;
void StartStrategyTest() {
  //set do all preparations here
  DBG_ONLY(Serial.println("Strategy Test start"));
  DBG_ONLY(delay(1000));

  //DBG_ONLY(calibrationEnabled=!calibrationEnabled);
  //DBG_ONLY(Serial.print("Calibration enabled:\t"));
  //DBG_ONLY(Serial.println(isCalibrationEnabled));
  cntr=0;
  SetHeadPos(35,5);
  delay(200);
  cmLeft=100;
  cmRight=100;
  //isCalibrationEnabled=0;
}

void RunStrategyTest() {
  
  if (currentPan<0)
    cmLeft = USonicFireAccurate();
  else 
    cmRight = USonicFireAccurate();
    
  SetHeadPan(-currentPan);
  delay(300);
  DisableHeadServos();
  if (currentPan<0)
    cmLeft = USonicFireAccurate();
  else 
    cmRight = USonicFireAccurate();

  // turn if obstacle
  if (cmLeft < 40 || cmRight < 40){
    if (cmRight < cmLeft)
      TurnLeft(30);
    else 
      TurnRight(30);
  }
  else 
    MoveForward(30);
  DBG_ONLY(Serial.print("dist:\t"));
  DBG_ONLY(Serial.print(cmLeft));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.println(cmRight));
  delay(1000);
  StopMoving();

  //if (cmLeft - cm Right > )
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
  
  if (cntr<60)
    cntr++;
  else   
    SetMode(0);
}

void FinishStrategyTest() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strategy Test finish"));
  
  StopMoving();
  CenterHead();
}
