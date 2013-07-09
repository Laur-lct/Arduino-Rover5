//this is test robot strategy to show the principle.

int blinkerDelay=0;
void StartStrategyBlinker() {
  //set do all preparations here
  blinkerDelay=200;
}

void RunStrategyBlinker() {
  statusLED1->toggle();
  statusLED2->toggle();
  delay(200);
}

void FinishStrategyBlinker() {
  //finish stuff before end
  statusLED1->off();
  statusLED2->off();
}
