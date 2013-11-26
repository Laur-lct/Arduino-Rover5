//this is test robot strategy to try new things.
int cntr=0;
int maxDistPan;
int maxDist;
int distTotal;
byte aboveTresholdSerie;
char delta;
const byte minDistTreshold=30;
boolean flg=false;
void StartStrategyTest() {
  //set do all preparations here
  DBG_ONLY(Serial.println("Strategy Test start"));
  DBG_ONLY(delay(1000));
  cntr=0;
  delta=15;
  aboveTresholdSerie=0;
  distTotal=0;
  maxDistPan=0;
  maxDist=0;
  SetHeadPos(-15,5);
  delay(200);
}

void RunStrategyTest() {
  int curDistIR;
  int curDistUS;
  GetDistanceUSandIR(&curDistIR,&curDistUS);
  int curDist = min(curDistIR, curDistUS);
//  DBG_ONLY(Serial.print("Pan angle = dist IR/US:\t"));
//  DBG_ONLY(Serial.print((int)currentPan));
//  DBG_ONLY(Serial.print(" = "));
//  DBG_ONLY(Serial.print(curDistIR));
//  DBG_ONLY(Serial.print("/"));
//  DBG_ONLY(Serial.println(curDistUS));
  if (curDist < minDistTreshold){
    if(aboveTresholdSerie>3 && maxDist < distTotal/aboveTresholdSerie){
      maxDist = distTotal/aboveTresholdSerie;
      maxDistPan = currentPan - delta*(aboveTresholdSerie/2);
    }
    aboveTresholdSerie=0;
    distTotal=0;
    if (abs(delta)==15)
      StopMoving();
  }
  else {
    aboveTresholdSerie++;
    distTotal+=curDist + (currentPan < 15 && currentPan > -15 ? 10:0);
  }
  if(!flg)
  SetHeadPan(currentPan+delta);
  if (abs(delta)!=15)
    delay(40);
  else 
    delay(140);
  
  
  if ((abs(delta)==10 && (currentPan<=(HEAD_REL_PAN_MIN) || currentPan>=(HEAD_REL_PAN_MAX))) || (abs(delta)==15 && (currentPan<=-15 || currentPan>=15))){
    if (!flg)
      flg=true;
    else {
      flg=false;
      if (abs(delta)==15 && aboveTresholdSerie<3){
        SetHeadPan(currentPan <=0 ? HEAD_REL_PAN_MIN : HEAD_REL_PAN_MAX);
        delta=(currentPan <=0 ? -10 : 10);
        delay(140);
      }
      else if (abs(delta)==15 && aboveTresholdSerie>=3 && !isMoving){
          DBG_ONLY(Serial.println("No obstacles. fwd"));
          MoveForward(30);
      }
      else if (abs(delta)==10){
        if(aboveTresholdSerie>3 && maxDist < distTotal/aboveTresholdSerie){
          maxDist = distTotal/aboveTresholdSerie;
          maxDistPan = currentPan - delta*(aboveTresholdSerie/2);
        }
        DBG_ONLY(Serial.print("best angle / max dist:\t"));
        DBG_ONLY(Serial.print((int)maxDistPan));
        DBG_ONLY(Serial.print(" / "));
        DBG_ONLY(Serial.println(maxDist));
        DBG_ONLY(Serial.println("Decision:"));
        if (maxDist< 40){
          DBG_ONLY(Serial.println("Dead end. Turning right"));
          TurnRight(30,90,true);
        }
        else if (maxDistPan < 19 && maxDistPan > -19){
          DBG_ONLY(Serial.println("Fwd"));
          MoveForward(30);
          SetHeadPan(currentPan <=0 ? -15 : 15);
          delta=(currentPan <=0 ? -15 : 15);
          delay(140);
        }
        else if (maxDistPan < 0){
          DBG_ONLY(Serial.println("Left"));
          TurnLeft(30,abs(maxDistPan),true);
          SetHeadPan(currentPan <=0 ? -15 : 15);
          delta=(currentPan <=0 ? -15 : 15);
        }
        else{ 
          DBG_ONLY(Serial.println("Right"));
          TurnRight(30,abs(maxDistPan),true);
          SetHeadPan(currentPan <=0 ? -15 : 15);
          delta=(currentPan <=0 ? -15 : 15);
        }
      }
      maxDist=0;
      aboveTresholdSerie=0;
      delta = -delta;
    }
  }
  if (cntr<2050)
    cntr++;
  else   
    SetMode(0);
}
void FinishStrategyTest() {
  //finish stuff before end
  DBG_ONLY(Serial.println("Strategy Test finish"));
  StopMoving();
  CenterHead();
}
