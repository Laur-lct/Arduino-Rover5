//all IR and US sensors related functions go here


//////////////////////////////////// IR ///////////////////////////////////////////////////
int milestoneIR[16];
// typically takes 70-110 miliseconds.
int RawIRMeasureAccurate(byte triesLeft=3) {
  int raws[5];
  const int maxDiff = 11;
  digitalWriteFast(PO_IRANALOG_SWITCH, true);
  delay(10);
  // the idea of filtering algorithm well explained here:   http://habrahabr.ru/post/167177/
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
  
  digitalWriteFast(PO_IRANALOG_SWITCH, false);
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
  int raw = RawIRMeasureAccurate(maxTries);
  return IRrawToCm(raw);
}
int IRrawToCm(int raw){
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
    milestoneIR[i] = EEPROM_READ_INT (MEMADDR_IRCACHE_START + i*2);
    //DBG_ONLY(Serial.print(milestoneIR[i]));
    //DBG_ONLY(Serial.print(' '));
  }
  //DBG_ONLY(Serial.println());
}

void DropIRCache() {
  for (int i = MEMADDR_IRCACHE_START; i < MEMADDR_IRCACHE_END; i++) 
    EEPROM.write(i,0);
}


////////////////////////////////// US /////////////////////////////////////////
// returns distance in cm
int USonicFire(){
  return microsecondsToCentimeters(USonicDoRawMeasure());
}

// returns more precise/reliable distance in cm. Takes more time to measure.
int USonicFireAccurate(byte triesLeft=3){
  long raws[3];
  const int maxDiff = 29*3; // 3 cm
  raws[0] = USonicDoRawMeasure();
  delay(30);
  raws[1] = USonicDoRawMeasure();
  delay(30);
  raws[2] = USonicDoRawMeasure();
    
  if (triesLeft==0 || (raws[0] > 60 && raws[0]-raws[1] > -maxDiff && raws[0]-raws[1] < maxDiff && raws[0]-raws[2] > -maxDiff && raws[0]-raws[2] < maxDiff))
    return microsecondsToCentimeters((raws[0] + raws[1] + raws[2])/3);
  delay(40);
  return USonicFireAccurate(--triesLeft);
}

long USonicDoRawMeasure(){
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(PO_SONICSENSOR_TRIGGER, OUTPUT);
  digitalWrite(PO_SONICSENSOR_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(PO_SONICSENSOR_TRIGGER, HIGH);
  delayMicroseconds(5);
  digitalWrite(PO_SONICSENSOR_TRIGGER, LOW);
  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(PO_SONICSENSOR_TRIGGER, INPUT);
  MsTimer2::stop();
  long res = pulseIn(PO_SONICSENSOR_TRIGGER, HIGH,20000);
  MsTimer2::start();
  return res >0 ? res :20000;
}

int microsecondsToCentimeters(long microseconds){
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return (int)((float)microseconds / 29 / 2 +0.5); 
}


//////////////////////////// Shared ///////////////////////////////////

// measures IR and US distances simultaneously. Faster then one by one. Takes minimum 102 milliseconds.
void GetDistanceUSandIR (int *distIR, int *distUS, byte triesLeft=3){
  int rawsIR[5];
  long rawsUS[5];
  int maxDiffIR = 11;
  int maxDiffUS = 29*2;

  char candidateIndexIR=-1;
  char confidenceIR=0;
  char lastMeasureIdxIR=4;
  char candidateIndexUS=-1;
  char confidenceUS=0;
  char lastMeasureIdxUS=4;
  digitalWriteFast(PO_IRANALOG_SWITCH, true);
  delay(10);
  for (char i=0; i<5; i++){
    if (lastMeasureIdxIR==4){
      rawsIR[i] = analogRead(PA_IRANALOG);
      if (confidenceIR == 0) {
        candidateIndexIR = i;
        confidenceIR++;
      }
      else if (rawsIR[candidateIndexIR]-rawsIR[i] > -maxDiffIR && rawsIR[candidateIndexIR]-rawsIR[i] < maxDiffIR) 
        confidenceIR++;
      else 
        confidenceIR--;
        
      if (confidenceIR> 5/2)
        lastMeasureIdxIR=i;
    }
    if (lastMeasureIdxUS==4){
      rawsUS[i] = USonicDoRawMeasure();
      if (confidenceUS == 0) {
        candidateIndexUS = i;
        confidenceUS++;
        maxDiffUS = 29*2+rawsUS[candidateIndexUS]*0.01;
      }
      else if (rawsUS[i] > 60 && rawsUS[candidateIndexUS]-rawsUS[i] > -maxDiffUS && rawsUS[candidateIndexUS]-rawsUS[i] < maxDiffUS) 
        confidenceUS++;
      else 
        confidenceUS--;
        
      if (confidenceUS> 5/2)
        lastMeasureIdxUS=i;
    }
    if (lastMeasureIdxIR < 4 && lastMeasureIdxUS < 4)
      break;
    else if(i<4)
      delay(30);
  }
  
  long total=0;
  confidenceIR=0;
  confidenceUS=0;
  //DBG_ONLY(Serial.print("IR raws: ")); 
  for (char i=0; i<=lastMeasureIdxIR; i++){
    //DBG_ONLY(Serial.print(rawsIR[i]));
    //DBG_ONLY(Serial.print(" "));
    if (rawsIR[candidateIndexIR]-rawsIR[i] > -maxDiffIR && rawsIR[candidateIndexIR]-rawsIR[i] < maxDiffIR){
      confidenceIR++;
      total+=rawsIR[i];
    }
  }
  //DBG_ONLY(Serial.println());
  *distIR = IRrawToCm((int)(total/confidenceIR));
  total=0;
  //DBG_ONLY(Serial.print("US raws: ")); 
  for (char i=0; i<=lastMeasureIdxUS; i++){
    //DBG_ONLY(Serial.print(rawsUS[i]));
    //DBG_ONLY(Serial.print(" "));
    if (rawsUS[i] > 60 && rawsUS[candidateIndexUS]-rawsUS[i] > -maxDiffUS && rawsUS[candidateIndexUS]-rawsUS[i] < maxDiffUS){
      confidenceUS++;
      total+=rawsUS[i];
    }
  }
  //DBG_ONLY(Serial.println());
  *distUS = microsecondsToCentimeters((int)(total/confidenceUS));
  if (!(confidenceIR > 5/2 && confidenceUS > 5/2) && triesLeft>0)
    GetDistanceUSandIR(distIR, distUS, --triesLeft);
}

void IRCalibration(){
  int milestoneIndex= (80-5)/5;
  CenterHead();
  delay(40);
  while (milestoneIndex>=0 && milestoneIndex < 16) {
    //stop at proper distance
    int currDistance = USonicFireAccurate();
    while (currDistance != milestoneIndex *5 + 5) {
      if (currDistance > milestoneIndex *5 + 5)
        MoveForward(10);
      else 
        MoveBackward(10);
      delay(100);
      currDistance = USonicFireAccurate();
      if (isModeUpdated)
        return;
    }
    StopMoving();
    delay(1000);
    if (isModeUpdated)
      return;
    //do IR raw measure and store it
    milestoneIR[milestoneIndex]=RawIRMeasureAccurate();
    DBG_ONLY(Serial.print("idx / sonic / IRraw:\t"));
    DBG_ONLY(Serial.print(milestoneIndex));
    DBG_ONLY(Serial.print(" / "));
    DBG_ONLY(Serial.print(currDistance));
    DBG_ONLY(Serial.print(" / "));
    DBG_ONLY(Serial.println(milestoneIR[milestoneIndex]));
 
  // ensure previous value was LESS then current and go to next step
  if (milestoneIndex == 15 || milestoneIR[milestoneIndex+1] < milestoneIR[milestoneIndex])
    milestoneIndex--;
  }
  for(int i=0; i<16; i++){
    EEPROM.write(MEMADDR_IRCACHE_START + i*2,(byte)((milestoneIR[i] >> 0) & 0xFF));
    EEPROM.write(MEMADDR_IRCACHE_START + i*2+1,(byte)((milestoneIR[i] >> 8) & 0xFF));
  }
}
