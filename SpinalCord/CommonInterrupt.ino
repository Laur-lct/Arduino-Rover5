// General service interrupt routine and variables here.
// measures all continuous sensors values like battery voltage, IRBumper, light intensity etc. 
// we cannot read compass measurements here as it takes too much time

//management
//boolean measure

//variables
char commonInterruptCntr=0;
float currVoltage;
float minVoltage=10;

byte isCharging=0; //the higher value, the higher probability that we're trully charging. 
byte IRBumperValues[4];
int currentConsumption;

void InitServiceInterrupt(){
  IRBumperValues[0]=0;
  IRBumperValues[1]=0;
  IRBumperValues[2]=0;
  IRBumperValues[3]=0;
  MsTimer2::set(SERVICE_TIMER_INTERVAL,ServiceTimerRoutine);
  MsTimer2::start();
}

void DeactivateCommonInterrupt(){
  MsTimer2::stop();
}

void ServiceTimerRoutine(){
  //servo control
  //unsigned long ms = millis();
  SoftwareServo::refresh();
  commonInterruptCntr++;
  //voltage
  float newVoltage = (float)analogRead(PA_BATT_VOLTAGE)/101.38f;
  if (newVoltage<minVoltage){
    minVoltage=newVoltage;
    //low voltage protection
    if (minVoltage<5.0)
      SelfPowerOff();  
  }
  if (newVoltage > 7.9 && newVoltage - currVoltage > 0.01){
    isCharging++;
    minVoltage=10;
  }
  else if (newVoltage < 8 && isCharging>0)
    isCharging = 0;
  currVoltage = newVoltage;
  if(commonInterruptCntr%3!=0)
    return;
  //ir bumper
  if(digitalReadFast(PO_IRBUMPER_SWITCH)){
    
    if(!digitalReadFast(PI_IRBUMPER_TL)){ if (IRBumperValues[0]<50) IRBumperValues[0]++;}
    else if (IRBumperValues[0]>0) IRBumperValues[0]--;
    
    if(!digitalReadFast(PI_IRBUMPER_TR)){if (IRBumperValues[1]<50) IRBumperValues[1]++;}
    else if (IRBumperValues[1]>0) IRBumperValues[1]--;
    
    if(!digitalReadFast(PI_IRBUMPER_BR)){if (IRBumperValues[2]<50) IRBumperValues[2]++;}
    else if (IRBumperValues[2]>0) IRBumperValues[2]--;
    
    if(!digitalReadFast(PI_IRBUMPER_BL)){if (IRBumperValues[3]<50) IRBumperValues[3]++;}
    else if (IRBumperValues[3]>0) IRBumperValues[3]--;
  }
  
  //consumption of current
  currentConsumption = analogRead(PA_BATT_CURRENT) + analogRead(PA_BATT_CURRENT) + analogRead(PA_BATT_CURRENT) + analogRead(PA_BATT_CURRENT);
  currentConsumption/=4;
  
  //light intensity
  
  //compass
  //if (isCompassEnabled) {
  //  sei();
  //  my3IMU.getValues(rawCompassVals);
  //  deltaRobotAngle = currentRobotAngle;
  //  currentRobotAngle = RadiansToDegrees(CalculateHeadingTiltCompensated(rawCompassVals));
  //  deltaRobotAngle = currentRobotAngle - deltaRobotAngle;
  //}
  //DEBUG_PRINTLN(millis() - ms);
}
