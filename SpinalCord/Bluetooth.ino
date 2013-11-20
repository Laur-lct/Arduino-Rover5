//all bluetooth related functions will go here
unsigned long btBaudRate=38400;
byte btStatus=0; // 0 - non initialized or disabled; 1 - initialized; 2 - connected; 3 - error

unsigned long lastBtMillis;
unsigned long lastBtReceiveMillis;
byte isPacketStarted=false;
byte receivedData[64];
byte receivedDataIdx=0;

struct BtCommand {
  byte commandNumber;
  unsigned int packageNumber;
  byte ttl;
  byte isResult;
  byte argLengths[6];
  byte args[30];
};

BtCommand btInputBuffer[BT_COM_BUFFER_SIZE] = {0};
BtCommand btOutputBuffer[BT_COM_BUFFER_SIZE] = {0};
byte currentInputBufIdx=0;
byte currentOutputBufIdx=0;

void InitBluetooth(unsigned long baudRate){
  byte brNum =BaudToFlag(baudRate);
  btBaudRate=FlagToBaud(brNum);
  Serial2.begin(btBaudRate);
  Serial2.print("AT");
  delay(5);
  if (!Serial2.available()){
    btStatus=3;
    char str[9] = "AT+BAUD0";
    str[7] = 48 + brNum; //asci code
    for (byte i=8; i>0; i--){
      Serial2.begin(FlagToBaud(i));
      delay(10);
      Serial2.write(str);
      delay(10);
      if (Serial2.available()>0) break;
    }
  }
  DEBUG_PRINT("Bluetooth Init: ");
  while(Serial2.available()){
    brNum = Serial2.read();
    DEBUG_PRINT((char)brNum);
    btStatus=1;
  }
  DEBUG_PRINTLN(btStatus);
  Serial2.begin(btBaudRate);
  
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
  delay(10);
  sprintf(str, "AT+PIN%d",btCode);
  Serial2.print(str);
  delay(10);
  while(Serial2.available())
    Serial2.read();
}

void ProcessBLuetooth(){
  //error or disabled
  if (btStatus==0 || btStatus==3)
    return;
  lastBtMillis=millis();
  ParseReceived();
  unsigned long deltaMillis = lastBtMillis-lastBtReceiveMillis;
  if (deltaMillis > 3*BT_PING_INTERVAL)
    btStatus=1;
  else if (deltaMillis > BT_PING_INTERVAL)
    SendServiceCommand(0,0);
  else {
    btStatus=2;
    SendCommands();
  }
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
  // service paket syntax <!commandNumber,1 byte of data!>
  // commandNumber 0-10 are service commands, others are regular:
  // regular paket syntax <!commandNumber,pktNumber,isResponceFlag,6argsLength,data!>
  boolean startEncountered = false;
  boolean endEncountered = false;
  while (Serial2.available()){
    receivedData[receivedDataIdx] = (char)Serial2.read();
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
       && receivedData[receivedDataIdx-1]==BT_START_DELIMITER[1] 
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
      if (startEncountered)
        receivedDataIdx=3; //delim length
      // receive packet data
      else if (!endEncountered)
        receivedDataIdx++;
      // parse packet
      else {
        PacketToCommand();     
        receivedDataIdx=0;
        isPacketStarted=false;
      }
    }
  }
}
void PacketToCommand(byte *buffer, byte len){
  if (len<3+3+2) //minimal possible length
    return;
  
  //service packet
  if (len<3+3+6){ 
    if (buffer[3]<)
  }
  //byte estimatedPacketLength;
  
}
void SendServiceCommand(byte commandNumber, byte argument){
  byte out[6] = {BT_START_DELIMITER[0],BT_START_DELIMITER[1],commandNumber,argument,BT_END_DELIMITER[0],BT_END_DELIMITER[1]};
  Serial2.write(out,6);
}
void SendCommands(){
}

struct BtCommand* GetCommandPlace(BtCommand* buffer, byte bufLen){
  return btOutputBuffer;
}
