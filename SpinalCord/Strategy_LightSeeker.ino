
int lightLeft=1023;
int lightRight=1023;
byte cntrL;
byte cntrR;
void StartStrategyLightSeeker() {
  //set do all preparations here
  DBG_ONLY(Serial.println("Strat Seeker Start"));
  DBG_ONLY(delay(1000));
  lightLeft=1023;
  lightRight=1023;
  cntrL=0;
  cntrR=0;
}

void RunStrategyLightSeeker() {
  lightLeft = (lightLeft+analogRead(PA_LIGHTSENSOR_L))/2;
  lightRight = (lightRight+analogRead(PA_LIGHTSENSOR_R))/2;
  DBG_ONLY(Serial.print("Light: left="));
  DBG_ONLY(Serial.print(lightLeft));
  DBG_ONLY(Serial.print(", right="));
  DBG_ONLY(Serial.println(lightRight));
  
  if (lightLeft- lightRight > 6){
    cntrL++;
    if (cntrR > 0)
      cntrR--;
  }
  else if (lightRight - lightLeft > 6){
    cntrR++;
    if (cntrL > 0)
      cntrL--;
  }
  else {
    if (cntrL > 0)
      cntrL--;
    if(cntrR > 0)
      cntrR--;
  }
  
  if (cntrL > 20){
    cntrL--;
    TurnLeft(50);
  }
  else if (cntrR > 20){
    cntrR--;
    TurnRight(50);
  }
  else {
    StopMoving();
  }
  delay(100);
}

void FinishStrategyLightSeeker() {
  DBG_ONLY(Serial.println("Strat seeker finish"));
  StopMoving();
}
