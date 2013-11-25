void StartStrategyRemote() {
  //set do all preparations here
  DEBUG_PRINTLN("Strat Remote Start");
  DBG_ONLY(delay(500));
  DEBUG_PRINTLN("Waiting for incoming commands...");
}

void RunStrategyRemote() {
  if (!ProcessIncomingCommands())
    delay(100);
  statusLED1->toggle();
}

void FinishStrategyRemote() {
  DBG_ONLY(Serial.println("Strat remote finish"));
  StopMoving();
}
boolean ProcessIncomingCommands(){
  BtCommand *comm = GetNextBtCommand();
  if (!comm || comm->isResult)
    return false;
  DEBUG_PRINTLN("Got");
  byte argCount;
  BtCommandArgument* args = GetBtCommandArguments(comm, &argCount);
  
  switch (comm->commandNumber){
    case 10:  //move fvd
      MoveForward(*(byte*)args[0].valPtr,*(unsigned int*)args[1].valPtr,*(boolean*)args[2].valPtr);
    break;
    case 11:  //move backwd
      MoveBackward(*(byte*)args[0].valPtr,*(unsigned int*)args[1].valPtr,*(boolean*)args[2].valPtr);
    break;
    case 12:  //turn left
      TurnLeft(*(byte*)args[0].valPtr,*(unsigned int*)args[1].valPtr,*(boolean*)args[2].valPtr);
    break;
    case 13:  //turn right
      TurnRight(*(byte*)args[0].valPtr,*(unsigned int*)args[1].valPtr,*(boolean*)args[2].valPtr);
    break;
    case 14:  //stop moving
      StopMoving();
    break;
    case 15:  //move wheels
    break;
    default: 
      SendServiceCommand(3,comm->commandNumber);
      DEBUG_PRINT("unknown command number: ");
      DEBUG_PRINTLN(comm->commandNumber);
    break;
  }
  //indicate command processed
  comm->ttl=0;
  return true;
}
