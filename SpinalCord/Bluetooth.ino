//all bluetooth related functions will go here
unsigned long btBaudRate=38400;
byte btStatus=0; // 0 - non initialized or disabled; 1 - initialized; 2 - connected; 3 - error

unsigned long lastBtMillis;
unsigned long lastBtReceiveMillis;
unsigned int globalPacketNumber=0;
byte isPacketStarted=false;
byte receivedData[64];
byte receivedDataIdx=0;
const byte startDelimLength = strlen(BT_START_DELIMITER); 
const byte endDelimLength = strlen(BT_END_DELIMITER);

typedef struct BtCommand {
  byte commandNumber;
  unsigned int packageNumber;
  byte ttl;
  byte isResult;
  byte argLengths[6];
  byte args[30];
};

typedef struct BtCommandArgument {
  byte length;
  void *valPtr;
};

BtCommand btInputBuffer[BT_COM_BUFFER_SIZE];
BtCommand btOutputBuffer[BT_COM_BUFFER_SIZE];
// is set when smth received
boolean anyCommandToExecute=false;

void InitBluetooth(unsigned long baudRate){
  
  DEBUG_PRINT("Bluetooth Init: ");
  byte brNum =BaudToFlag(baudRate);
  btBaudRate=FlagToBaud(brNum);
  Serial2.begin(btBaudRate);
  Serial2.print("AT");
  delay(600);
  if (!Serial2.available()){
    btStatus=3;
    char str[9] = "AT+BAUD0";
    str[7] = 48 + brNum; //asci code
    for (byte i=8; i>0; i--){
      Serial2.begin(FlagToBaud(i));
      delay(50);
      Serial2.write(str);
      delay(600);
      if (Serial2.available()>0) break;
    }
  }
  
  while(Serial2.available()){
    brNum = Serial2.read();
    DEBUG_PRINT((char)brNum);
    btStatus=1;
  }
  DEBUG_PRINTLN(btStatus);
  Serial2.begin(btBaudRate);
  //reset all command buffers
  for (byte i=0; i<BT_COM_BUFFER_SIZE; i++){
    btInputBuffer[i].ttl=0;
    btOutputBuffer[i].ttl=0;
  }
}
// possible baudRate values
//1——1200    2——2400    3——4800    4——9600 
//5——19200  6——38400  7——57600  8——115200
unsigned long FlagToBaud(byte btFlag){
  if (btFlag==8) return 115200;
  if (btFlag==7) return 57600;
  if (btFlag==6) return 38400;
  if (btFlag==5) return 19200;
  if (btFlag==4) return 9600;
  if (btFlag==3) return 4800;
  if (btFlag==2) return 2400;
  if (btFlag==1) return 1200;
//else return default
  return 38400;
}
byte BaudToFlag(unsigned long baudRate){
  if (baudRate==115200) return 8;
  if (baudRate==57600) return 7;
  if (baudRate==38400) return 6;
  if (baudRate==19200) return 5;
  if (baudRate==9600) return 4;
  if (baudRate==4800) return 3;
  if (baudRate==2400) return 2;
  if (baudRate==1200) return 1;
  //else return default
  return 6;
}

void BtSetNameAndPass(char *btName, unsigned int btCode){
  if(btStatus!=1)
    return;
  char str[64];
  sprintf(str, "AT+NAME%s",btName);
  Serial2.print(str);
  delay(600);
  sprintf(str, "AT+PIN%d",btCode);
  Serial2.print(str);
  delay(600);
  while(Serial2.available())
    Serial2.read();
}

BtCommand* GetNextBtCommandPlace(BtCommand buffer[], byte bufLen=BT_COM_BUFFER_SIZE){
  byte minTtl=255;
  byte minTtlIndex=0;
  for (byte i=0; i<bufLen; i++){
    if (buffer[i].ttl==0)
      return &buffer[i];
    else if(buffer[i].ttl > BT_KEEP_TTL && buffer[i].ttl<minTtl){
      minTtl = buffer[i].ttl;
      minTtlIndex = i;
    }
  }
  return &buffer[minTtlIndex];
}

BtCommand* GetNextBtCommand(BtCommand buffer[]=btInputBuffer, byte bufLen=BT_COM_BUFFER_SIZE){
  if (!anyCommandToExecute)
    return NULL;
  byte minTtl=255;
  byte minTtlIndex=0;
  for (byte i=0; i<bufLen; i++){
    if(buffer[i].ttl > 0 && buffer[i].ttl<minTtl){
      minTtl = buffer[i].ttl;
      minTtlIndex = i; 
    }
  }
  if (minTtl==255) {
    anyCommandToExecute=false;
    return NULL;
  }
  return &buffer[minTtlIndex];
}

void CheckBtStatusAndPing(){
  unsigned long deltaMillis = lastBtMillis-lastBtReceiveMillis;
  if (deltaMillis > 5*BT_PING_INTERVAL){
    btStatus=1;
    SendServiceCommand(0,0);
  }
  else if (deltaMillis > BT_PING_INTERVAL)
    SendServiceCommand(0,0);
  else 
    btStatus=2;
}

void ProcessBluetooth(){
  
  //error or disabled
  if (btStatus==0 || btStatus==3)
    return;
  //TODO: process ttl here weighted on time
  lastBtMillis=millis();
  for (byte i=0; i<BT_COM_BUFFER_SIZE; i++){
    if (btInputBuffer[i].ttl > 0)
      btInputBuffer[i].ttl--;
    if (btOutputBuffer[i].ttl > 0)
      btOutputBuffer[i].ttl--;
  }
  ParseReceived();
  CheckBtStatusAndPing();
  if (btStatus==2)
    SendCommands();
}

void ParseReceived(){
  if (!Serial2.available())
    return;
  //clear old data in the buffer
  if (receivedDataIdx > 0  && lastBtMillis-lastBtReceiveMillis > 1000){
    receivedDataIdx=0;
    isPacketStarted=false;
  }
  lastBtReceiveMillis = lastBtMillis;
  btStatus=2;
  // service paket syntax <!commandNumber,2 bytes of data!>
  // commandNumber 0-10 are service commands, others are regular:
  // regular paket syntax <!commandNumber,pktNumber,isResponceFlag,6argsLength,data!>
  boolean startEncountered = false;
  boolean endEncountered = false;
  //DEBUG_PRINT("BT received: ");
  while (Serial2.available()){
    receivedData[receivedDataIdx] = (char)Serial2.read();
    //DEBUG_PRINT(receivedData[receivedDataIdx]);
    //DEBUG_PRINT(", ");
    //check for for package start
    if (receivedDataIdx > 1 
        && receivedData[receivedDataIdx]==BT_START_DELIMITER[2] 
        && receivedData[receivedDataIdx-1]==BT_START_DELIMITER[1] 
        && receivedData[receivedDataIdx-2]==BT_START_DELIMITER[0]){
      startEncountered = true;
      if (!isPacketStarted) isPacketStarted=true;
    }
    else if (receivedDataIdx > 1
       && receivedData[receivedDataIdx]==BT_END_DELIMITER[2]
       && receivedData[receivedDataIdx-1]==BT_END_DELIMITER[1] 
       && receivedData[receivedDataIdx-2]==BT_END_DELIMITER[0])
      endEncountered =true;
      
    if (!isPacketStarted){
      //ignore anything except of start delimiter
      if ((receivedDataIdx==0 && receivedData[receivedDataIdx]==BT_START_DELIMITER[0]) || (receivedDataIdx==1 && receivedData[receivedDataIdx]==BT_START_DELIMITER[1]))
        receivedDataIdx++;
      else 
        receivedDataIdx=0;
    }
    else {
      // found start more than once
      if (startEncountered){
        receivedDataIdx=3; //delim length
        startEncountered=false;
      }
      // receive packet data
      else if (!endEncountered){
        if (receivedDataIdx<64-3)
          receivedDataIdx++;
      }
      // parse packet
      else {
        if (receivedDataIdx > (endDelimLength + startDelimLength + 1))
          PacketToCommand(receivedData+startDelimLength, receivedDataIdx - endDelimLength - startDelimLength + 1);     
        receivedDataIdx=0;
        isPacketStarted=false;
      }
    }
  }
  //DEBUG_PRINTLN();
    
}

void PacketToCommand(byte *buffer, byte len){
#if defined(DEBUG)
  Serial.print("Raw packet data: ");
  for (byte i=0; i<len; i++){
    Serial.print(buffer[i]);
    Serial.print(", ");
  }
  Serial.print("len=");
  Serial.println(len);
#endif

  byte comNum = buffer[0];
  if ( len < 3 || (len >= 6 && comNum < 10) || len > 40){
    SendServiceCommand(2,*((int *)(&buffer[1])));
    DEBUG_PRINTLN("bad request!");
    return;
  }
  DEBUG_PRINT("Command: ");
  if (comNum==0) { //ping, reply ping, if that is asked
    if (buffer[1]>0)
      SendServiceCommand(0,1);
    DEBUG_PRINTLN("Ping");
  }
  else if (comNum==1) { // ok result. clear this packet from outgoing buffer
    DEBUG_PRINTLN("OK");
    int pkt = buffer[1]+(buffer[2]<<8);
    for (byte i=0; i<BT_COM_BUFFER_SIZE; i++){
      if (btOutputBuffer[i].packageNumber==pkt){
        btOutputBuffer[i].ttl=0;
        break;
      }
    }
  }
  else if (comNum==2) { // error on receive or bad request format) { resend the oldest outgoing command
    DEBUG_PRINTLN("Bad request");
    BtCommand *comm = GetNextBtCommand(btOutputBuffer);
    if (comm!= NULL && comm->ttl < BT_KEEP_TTL)
      SendRegularCommand(comm);
  }
  else if (comNum==3) { // unknown command number, nothing to do
    DEBUG_PRINTLN("unknown command for phone");
  }
//else if (comNum==4) { 
//}
else if (comNum==5) { //set robot mode
  SetMode(buffer[1]);
  DEBUG_PRINTLN("Set mode");
}
else if (comNum==6) { // power off
  DEBUG_PRINTLN("PowerOff Received");
  SelfPowerOff();
}
  else if (comNum==9) { // print command list
    //PrintCommandList();
  }
  else if (comNum>=10) { //translate to regular command
    BtCommand* comm = GetNextBtCommandPlace(btInputBuffer);
    comm->commandNumber = comNum;
    comm->packageNumber = buffer[1]+(buffer[2]<<8);
    comm->isResult=buffer[3];
    byte totalDataLength = 0;
    byte i=0;
    for (; i<6; i++){
      comm->argLengths[i]=buffer[i+4];
      totalDataLength+=buffer[i+4];
    }
    //bad request
    if (i+4+totalDataLength > len){
      comm->ttl=0;
      SendServiceCommand(2,comm->packageNumber);
      DEBUG_PRINTLN("bad request!");
    } 
    else { // all ok for now,copy data
      memcpy(comm->args, buffer+i+4, totalDataLength);
      SendServiceCommand(1,comm->packageNumber); //send OK
      comm->ttl=254;
      anyCommandToExecute = true;
      DEBUG_PRINTLN(comNum);
    }
  }
  else { //send command unknown
    SendServiceCommand(3,comNum);
    DEBUG_PRINT(comNum);
    DEBUG_PRINTLN(" unknown service command number");
  }
}

void SendServiceCommand(byte commandNumber, int argument){
  Serial2.write(BT_START_DELIMITER);
  Serial2.write(commandNumber);
  Serial2.write(argument);
  Serial2.write(BT_END_DELIMITER);
}

void SendRegularCommand(struct BtCommand *comm){
  //if this is first send
  if (comm->ttl > BT_KEEP_TTL){
    comm->ttl = BT_KEEP_TTL;
    comm->packageNumber=globalPacketNumber++;
  }
  byte totalDataLength=0;
  for (byte i=0; i<6; i++)
    totalDataLength +=comm->argLengths[i];
    
  Serial2.write(BT_START_DELIMITER);
  Serial2.write(comm->commandNumber);
  Serial2.write(comm->packageNumber);
  Serial2.write(comm->isResult);
  Serial2.write(comm->argLengths,6);
  Serial2.write(comm->args,totalDataLength);
  Serial2.write(BT_END_DELIMITER);
}

//this is used in strategies
void EnqueueBtCommand(byte commNumber, boolean isResponse, 
                        byte argLen1, void *argValPtr1, 
                        byte argLen2=0, void *argValPtr2=0, 
                        byte argLen3=0, void *argValPtr3=0,
                        byte argLen4=0, void *argValPtr4=0,
                        byte argLen5=0, void *argValPtr5=0, 
                        byte argLen6=0, void *argValPtr6=0){
  BtCommand *comm = GetNextBtCommandPlace(btOutputBuffer);
  byte totalDataLength=0;
  comm->commandNumber = commNumber;
  comm->isResult = isResponse;
  comm->argLengths[0] = argLen1;
  memcpy(comm->args, argValPtr1, argLen1);
  if (argLen2!=0){
    totalDataLength+=argLen1;
    comm->argLengths[1] = argLen2;
    memcpy(comm->args+totalDataLength,argValPtr2, argLen2);
    if (argLen3!=0){
      totalDataLength+=argLen2;
      comm->argLengths[2] = argLen3;
      memcpy(comm->args+totalDataLength, argValPtr3, argLen3);
      if (argLen4!=0){
        totalDataLength+=argLen3;
        comm->argLengths[3] = argLen4;
        memcpy(comm->args+totalDataLength, argValPtr4, argLen4);
        if (argLen5!=0){
          totalDataLength+=argLen4;
          comm->argLengths[4] = argLen5;
          memcpy(comm->args+totalDataLength, argValPtr5, argLen5);
          if (argLen6!=0){
            totalDataLength+=argLen5;
            comm->argLengths[5] = argLen6;
            memcpy(comm->args+totalDataLength, argValPtr6, argLen6);
          }
        }
      }
    }
  } 
  comm->ttl = 254;
}

//returns number of arguments and array of pointers to each argument
struct BtCommandArgument* GetBtCommandArguments(BtCommand *comm, byte* totalArgsNum){
  byte totalDataLength=0;
  BtCommandArgument args[6];
  (*totalArgsNum)=0;
  for (byte i=0; i<6; i++){
    if (comm->argLengths[i] > 0){
      (*totalArgsNum)++;
      args[i].length = comm->argLengths[i];
      args[i].valPtr = comm->args+totalDataLength;
      totalDataLength+=comm->argLengths[i];
    }
    else {
      args[i].length = 0;
      args[i].valPtr = NULL;
    }
  }
  return args;
}

void SendCommands(){
  for (byte i=0; i<BT_COM_BUFFER_SIZE; i++)
    if (btOutputBuffer[i].ttl > BT_KEEP_TTL)
      SendRegularCommand(&btOutputBuffer[i]);
}
