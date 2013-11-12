//this is test robot strategy to show the principle.

int blinkerDelay;
int blinkerCntr;
void StartStrategyBlinker() {
  //do all preparations here
  DEBUG_PRINTLN("Strat blinker Start");
  DEBUG_PRINT("freeMemory()=");
  DEBUG_PRINTLN(freeMemory());
  DBG_ONLY(delay(1000));
  blinkerDelay=250;
  blinkerCntr=0;
  delay(20);
}

void RunStrategyBlinker() {
  statusLED2->setValue(1);
  //DBG_ONLY(Serial.println("blinking..."));
  //DBG_ONLY(Serial.print("Batt voltage="));
  //unsigned long ms = millis();
  //my3IMU.getValues(rawCompassVals);
  //RadiansToDegrees(CalculateHeadingTiltCompensated(rawCompassVals));
  //DEBUG_PRINTLN(millis() - ms);
  //DBG_ONLY(Serial.print("Batt voltage="));
  /*DBG_ONLY(Serial.println(currVoltage));
  DBG_ONLY(Serial.print("current draw="));
  DBG_ONLY(Serial.println(currentConsumption));
  DBG_ONLY(Serial.print("currentDraw = "));
  DBG_ONLY(Serial.print(currentDraw[0]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(currentDraw[1]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(currentDraw[2]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.println(currentDraw[3]));*/
  
  delay(blinkerDelay);
  statusLED2->off();
  delay(blinkerDelay);
  
  // turn off robot if nothing happening
  if (blinkerCntr>1000)
    SelfPowerOff();
  else 
    blinkerCntr++;
}

void FinishStrategyBlinker() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strat blinker finish"));
  statusLED2->off();
}
