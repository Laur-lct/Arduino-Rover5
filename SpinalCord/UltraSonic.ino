// all ultrasonic functions here

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
  long res = pulseIn(PO_SONICSENSOR_TRIGGER, HIGH,20000);
  return res >0 ? res :20000;
}

int microsecondsToCentimeters(long microseconds){
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return (int)((float)microseconds / 29 / 2 +0.5); 
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
