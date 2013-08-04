// all ultrasonic functions here

// returns distance in cm
int USonicFire(){
  return microsecondsToCentimeters(USonicDoRawMeasure());
}

// returns more precise/reliable distance in cm. Takes more time to measure.
int USonicFireAccurate(){
  long raws[3];
  byte maxTries=3;
  int maxDiff = 29*3; // 3 cm
  
  while (maxTries>0){
    raws[0] = USonicDoRawMeasure();
    delay(15);
    raws[1] = USonicDoRawMeasure();
    delay(15);
    raws[2] = USonicDoRawMeasure();
    
    if (raws[0] > 90 && raws[0]-raws[1] > - maxDiff && raws[0]-raws[1] < maxDiff && raws[0]-raws[2] > - maxDiff && raws[0]-raws[2] < maxDiff)
      return microsecondsToCentimeters((raws[0] + raws[1] + raws[2])/3);
    delay(20);
    maxTries--;
  }
  return 340;
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
  return pulseIn(PO_SONICSENSOR_TRIGGER, HIGH,20000);
}

int microsecondsToCentimeters(long microseconds){
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
