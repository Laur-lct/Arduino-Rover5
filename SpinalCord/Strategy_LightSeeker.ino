
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
  //DBG_ONLY(Serial.print("Light: left="));
  //DBG_ONLY(Serial.print(lightLeft));
  //DBG_ONLY(Serial.print(", right="));
  //DBG_ONLY(Serial.println(lightRight));
  
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
    digitalWrite(PO_MOTOR_DIR_TL,!MOTOR_FWD_TL);
    digitalWrite(PO_MOTOR_DIR_TR,MOTOR_FWD_TR);
    digitalWrite(PO_MOTOR_DIR_BL,!MOTOR_FWD_BL);
    digitalWrite(PO_MOTOR_DIR_BR,MOTOR_FWD_BR);
    analogWrite(PP_MOTOR_SPD_TL,60);
    analogWrite(PP_MOTOR_SPD_TR,60);
    analogWrite(PP_MOTOR_SPD_BL,60);
    analogWrite(PP_MOTOR_SPD_BR,60);
  }
  else if (cntrR > 20){
    cntrR--;
    digitalWrite(PO_MOTOR_DIR_TL,MOTOR_FWD_TL);
    digitalWrite(PO_MOTOR_DIR_TR,!MOTOR_FWD_TR);
    digitalWrite(PO_MOTOR_DIR_BL,MOTOR_FWD_BL);
    digitalWrite(PO_MOTOR_DIR_BR,!MOTOR_FWD_BR);
    analogWrite(PP_MOTOR_SPD_TL,60);
    analogWrite(PP_MOTOR_SPD_TR,60);
    analogWrite(PP_MOTOR_SPD_BL,60);
    analogWrite(PP_MOTOR_SPD_BR,60);
  }
  else {
    analogWrite(PP_MOTOR_SPD_TL,0);
    analogWrite(PP_MOTOR_SPD_TR,0);
    analogWrite(PP_MOTOR_SPD_BL,0);
    analogWrite(PP_MOTOR_SPD_BR,0);
  }
  delay(100);
}

void FinishStrategyLightSeeker() {
  DBG_ONLY(Serial.println("Strat seeker finish"));
  DBG_ONLY(delay(1000));
}
