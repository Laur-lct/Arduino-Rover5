//all sensors related functions will go here

int milestoneIR[16];

// typically takes 60-100 miliseconds.
int RawIRMeasureAccurate(byte triesLeft=3) {
  int raws[5];
  const int maxDiff = 11;
  if (!digitalRead(PO_IRANALOG_SWITCH)){
    digitalWrite(PO_IRANALOG_SWITCH, true);
    delay(30);
  }
  // the idea of filtering algorithm well expleined here:   http://habrahabr.ru/post/167177/
  char candidateIndex=-1;
  char confidence=0;
  char lastMeasureIdx=4;
  for (char i=0; i<5; i++){
    raws[i] = analogRead(PA_IRANALOG);
    if (confidence == 0) {
      candidateIndex = i;
      confidence++;
    }
    else if (raws[candidateIndex]-raws[i] > -maxDiff && raws[candidateIndex]-raws[i] < maxDiff) 
      confidence++;
    else 
      confidence--;
    if (confidence> 5/2){
      lastMeasureIdx=i;
      break;
    }
    delay(20);
  }
  int total=0;
  confidence=0;
  //DBG_ONLY(Serial.print("IR raws: ")); 
  for (char i=0; i<=lastMeasureIdx; i++){
    //DBG_ONLY(Serial.print(raws[i]));
    //DBG_ONLY(Serial.print(" "));
    if (raws[candidateIndex]-raws[i] > -maxDiff && raws[candidateIndex]-raws[i] < maxDiff){
      confidence++;
      total+=raws[i];
    }
  }
  //DBG_ONLY(Serial.println());  
  if (confidence > 5/2 || triesLeft==0)
    return total/confidence;
  return RawIRMeasureAccurate(--triesLeft);
}

// returns distance in cm. from 5 to 80
int GetDistanceByIR(byte maxTries=3){
  if (!digitalRead(PO_IRANALOG_SWITCH)){
    digitalWrite(PO_IRANALOG_SWITCH, true);
    delay(40);
  }
  int raw = maxTries==0 ? analogRead(PA_IRANALOG) : RawIRMeasureAccurate(maxTries);
  if (raw >= milestoneIR[0]) // real distance less or equal 5cm
    return 5;
  int milestoneIndex=1;

  //else lookup
  for (; milestoneIndex < 16; milestoneIndex++){
    if (milestoneIR[milestoneIndex] <= raw)
      break;
  }
  if (milestoneIndex==16) // something was beyond 80 cm
    return 80;
  //then calculate
  float weight = (float)(raw - milestoneIR[milestoneIndex]) / (milestoneIR[milestoneIndex-1] - milestoneIR[milestoneIndex]);
  return (int)(weight * ((milestoneIndex-1)*5) + (1-weight)*(milestoneIndex*5) + 0.5 + 5); 
}

void InitIRSensor(){
  //DBG_ONLY(Serial.println("IR cache:"));
  for(int i=0; i<16; i++){
    milestoneIR[i] = (int)(((EEPROM.read(MEMADDR_IRCACHE_START + i*2) << 0) & 0xFF) + ((EEPROM.read(MEMADDR_IRCACHE_START + i*2 + 1) << 8) & 0xFF00));
    //DBG_ONLY(Serial.print(milestoneIR[i]));
    //DBG_ONLY(Serial.print(' '));
  }
  //DBG_ONLY(Serial.println());
}

void DropIRCache() {
  for (int i = MEMADDR_IRCACHE_START; i < MEMADDR_IRCACHE_END; i++) 
    EEPROM.write(i,0);
}
