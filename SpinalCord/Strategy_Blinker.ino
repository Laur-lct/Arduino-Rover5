//this is test robot strategy to show the principle.

int blinkerDelay=200;
void StartStrategyBlinker() {
  //set do all preparations here
  DBG_ONLY(Serial.println("Strat blinker Start"));
  
  DBG_ONLY(Serial.print("freeMemory()="));
  DBG_ONLY(Serial.println(freeMemory()));

  DBG_ONLY(delay(1000));
  blinkerDelay=200;
  PrintAllCache();
}

void RunStrategyBlinker() {
  statusLED2->setValue(1);
  //DBG_ONLY(Serial.println("blinking..."));
  //DBG_ONLY(Serial.print("Batt voltage="));
  delay(blinkerDelay);
  statusLED2->off();
  delay(blinkerDelay);
}

void FinishStrategyBlinker() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strat blinker finish"));
  statusLED2->off();
}
