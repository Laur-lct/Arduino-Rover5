//this is test robot strategy to show the principle.

int blinkerDelay=200;
void StartStrategyBlinker() {
  //set do all preparations here
  DBG_ONLY(Serial.println("Strat blinker Start"));
  DBG_ONLY(delay(1000));
  blinkerDelay=200;
}

void RunStrategyBlinker() {
  statusLED2->setValue(1);
  //DBG_ONLY(Serial.println("blinking..."));
  //DBG_ONLY(Serial.print("Batt voltage="));
  //DBG_ONLY(Serial.print(analogRead(PA_BATT_VOLTAGE)));
  //DBG_ONLY(Serial.print(" curr="));
  //DBG_ONLY(Serial.print(analogRead(PA_BATT_CURRENT)));
  //DBG_ONLY(Serial.print(" IR="));
  //DBG_ONLY(Serial.println(analogRead(PA_IRANALOG)));
  //DBG_ONLY(Serial.print("ENC TL="));
  //DBG_ONLY(Serial.println(totalPathOnTL));
  delay(blinkerDelay);
  statusLED2->off();
  delay(blinkerDelay);
}

void FinishStrategyBlinker() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strat blinker finish"));
  statusLED2->off();
}
