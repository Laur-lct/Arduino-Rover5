//all sensors related functions will go here

int milestoneIR[15];

int RawIRMeasure(byte triesLeft=3) {
  digitalWrite(PO_IRANALOG_SWITCH, 1);
  long raws[3];
  const int maxDiff = 30;
  delay(40);
  raws[0] = analogRead(PA_IRANALOG);
  delay(40);
  raws[1] = analogRead(PA_IRANALOG);
  delay(40);
  raws[2] = analogRead(PA_IRANALOG);
    
  if (triesLeft==0 || (raws[0]-raws[1] > -maxDiff && raws[0]-raws[1] < maxDiff && raws[0]-raws[2] > -maxDiff && raws[0]-raws[2] < maxDiff))
    return ((raws[0] + raws[1] + raws[2])/3);
  else 
 return RawIRMeasure(--triesLeft);
}

int GetDistanceByIR(){
  int raw = RawIRMeasure();
  int milestoneIndex=1;
  if (raw >= milestoneIR[0]) // real distance less or equal 10cm
    return 10;
  //else lookup
  for (; milestoneIndex < 15; milestoneIndex++){
 if (milestoneIR[milestoneIndex] <= raw)
   break;
  }
  if (milestoneIndex==15) // something was beyond 80 cm
    return 80;
  //then calculate
  float weight = (float)(milestoneIR[milestoneIndex-1] - raw) / (milestoneIR[milestoneIndex-1] - milestoneIR[milestoneIndex]);
  return (int)(weight * milestoneIR[milestoneIndex-1] + (1-weight)*milestoneIR[milestoneIndex] + 0.5); 
}

void IRCalibration(){
  int milestoneIndex= (80-10)/5;
  while (milestoneIndex>=0 || milestoneIndex < 15) {
    //stop at proper distance
 int currDistance = USonicFireAccurate();
 while (currDistance != milestoneIndex *5 ) {
   if (currDistance > milestoneIndex *5)
     MoveForward(10);
   else 
     MoveBackward(10);
      delay(500);
      StopMoving();
      currDistance = USonicFireAccurate();
    }
 //do IR raw measure and store it
 milestoneIR[milestoneIndex]=RawIRMeasure();
 // ensure previous value was LESS then current and go to next step
 if (milestoneIndex < 14 && milestoneIR[milestoneIndex+1] < milestoneIR[milestoneIndex])
   milestoneIndex--;
  }
}
