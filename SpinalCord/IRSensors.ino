//all sensors related functions will go here

int milestoneIR[16];
int RawIRMeasureAccurate(byte triesLeft=3) {
  long raws[3];
  const int maxDiff = 16;
  if (!digitalRead(PO_IRANALOG_SWITCH)){
    digitalWrite(PO_IRANALOG_SWITCH, true);
    delay(40);
  }
  raws[0] = analogRead(PA_IRANALOG);
  delay(40);
  raws[1] = analogRead(PA_IRANALOG);
  delay(40);
  raws[2] = analogRead(PA_IRANALOG);
    
  if (triesLeft==0 || (raws[0]-raws[1] > -maxDiff && raws[0]-raws[1] < maxDiff && raws[0]-raws[2] > -maxDiff && raws[0]-raws[2] < maxDiff))
    return (raws[0] + raws[1] + raws[2])/3;
  return RawIRMeasureAccurate(--triesLeft);
}

// returns distance in cm. from 5 to 80
int GetDistanceByIR(byte maxTries=0){
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
  float weight = (float)(milestoneIR[milestoneIndex-1] - raw) / (milestoneIR[milestoneIndex-1] - milestoneIR[milestoneIndex]);
  return (int)(weight * milestoneIR[milestoneIndex-1] + (1-weight)*milestoneIR[milestoneIndex] + 0.5); 
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
