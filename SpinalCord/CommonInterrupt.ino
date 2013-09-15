// General service interrupt routine and variables here.
// measures all continuous sensors values like battery voltage, IRBumper, light intensity, magnetometer, accelerometer, etc. 

//management
//boolean measure

//variables
char commonInterruptCntr=0;
float currVoltage;
float minVoltage=10;

byte isCharging=0; //the higher value, the higher probability that we're trully charging. 
byte IRBumperValues[4];

float currAngle=0;
float deltaAngle=0;

int currentConsumption;

void InitServiceInterrupt(){
  IRBumperValues[0]=0;
  IRBumperValues[1]=0;
  IRBumperValues[2]=0;
  IRBumperValues[3]=0;
  MsTimer2::set(SERVICE_TIMER_INTERVAL,ServiceTimerRoutine);
  MsTimer2::start();
}

void ServiceTimerRoutine(){
  //servo control
  SoftwareServo::refresh();
  
  commonInterruptCntr++;
  if(commonInterruptCntr%3!=0)
    return;
  //voltage
  float newVoltage = (float)analogRead(PA_BATT_VOLTAGE)/101.38f;
  if (newVoltage - currVoltage > 0.01)
    isCharging++;
  else if (isCharging>0)
    isCharging--;
  currVoltage = newVoltage;
  if (currVoltage<minVoltage)
    minVoltage=currVoltage;
  
  //ir bumper
  if(digitalReadFast(PO_IRBUMPER_SWITCH)){
    if(!digitalReadFast(PI_IRBUMPER_TL)){
      if (IRBumperValues[0]<255) IRBumperValues[0]++;
    }
    else if (IRBumperValues[0]>0)
      IRBumperValues[0]--;
    if(!digitalReadFast(PI_IRBUMPER_TR)){
      if (IRBumperValues[1]<255) IRBumperValues[1]++;
    }
    else if (IRBumperValues[1]>0)
      IRBumperValues[1]--;
    if(!digitalReadFast(PI_IRBUMPER_BR)){
      if (IRBumperValues[2]<255) IRBumperValues[2]++;
    }
    else if (IRBumperValues[2]>0)
      IRBumperValues[2]--;
    if(!digitalReadFast(PI_IRBUMPER_BL)){
      if (IRBumperValues[3]<255) IRBumperValues[3]++;
    }
    else if (IRBumperValues[3]>0)
      IRBumperValues[3]--;
  }
  
  //consumption of current
  currentConsumption = analogRead(PA_BATT_CURRENT);
  currentConsumption+= analogRead(PA_BATT_CURRENT);
  currentConsumption+= analogRead(PA_BATT_CURRENT);
  currentConsumption+= analogRead(PA_BATT_CURRENT);
  currentConsumption/=4;
  //compass
  
  //light intensity
  
}
