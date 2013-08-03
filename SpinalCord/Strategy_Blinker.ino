//this is test robot strategy to show the principle.

int blinkerDelay;
void StartStrategyBlinker() {
  //do all preparations here
  DBG_ONLY(Serial.println("Strat blinker Start"));
  DBG_ONLY(Serial.print("freeMemory()="));
  DBG_ONLY(Serial.println(freeMemory()));
  DBG_ONLY(delay(1000));
  
  blinkerDelay=200;
  //SetHeadPan(-110);
}

void RunStrategyBlinker() {
  statusLED2->setValue(1);
  //DBG_ONLY(Serial.println("blinking..."));
  //DBG_ONLY(Serial.print("Batt voltage="));
  delay(blinkerDelay);
  statusLED2->off();
  delay(blinkerDelay);
  SetHeadTilt(currentTilt+1);
  DBG_ONLY(Serial.print("Head pan = "));
  DBG_ONLY(Serial.print((int)currentPan));
  DBG_ONLY(Serial.print("\ttilt = "));
  DBG_ONLY(Serial.println((int)currentTilt));
}

void FinishStrategyBlinker() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strat blinker finish"));
  statusLED2->off();
}
