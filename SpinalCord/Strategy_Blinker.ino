//this is test robot strategy to show the principle.

int blinkerDelay;
void StartStrategyBlinker() {
  //do all preparations here
  DBG_ONLY(Serial.println("Strat blinker Start"));
  DBG_ONLY(Serial.print("freeMemory()="));
  DBG_ONLY(Serial.println(freeMemory()));
  DBG_ONLY(delay(1000));
  //DBG_ONLY(PrintAllCache());
  blinkerDelay=250;  
}

void RunStrategyBlinker() {
  statusLED2->setValue(1);
  //DBG_ONLY(Serial.println("blinking..."));
  //DBG_ONLY(Serial.print("Batt voltage="));
  int ir, us;
  GetDistanceUSandIR(&ir,&us);
  /*DBG_ONLY(Serial.print("Batt voltage="));
  DBG_ONLY(Serial.println(currVoltage));
  DBG_ONLY(Serial.print("current draw="));
  DBG_ONLY(Serial.println(currentConsumption));
  DBG_ONLY(Serial.print("IRBumper = "));
  DBG_ONLY(Serial.print(IRBumperValues[0]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(IRBumperValues[1]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.print(IRBumperValues[2]));
  DBG_ONLY(Serial.print('\t'));
  DBG_ONLY(Serial.println(IRBumperValues[3]));*/
  delay(blinkerDelay);
  statusLED2->off();
  delay(blinkerDelay);
}

void FinishStrategyBlinker() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strat blinker finish"));
  statusLED2->off();
}
