//all bluetooth related functions will go here
unsigned long btBaudRate=38400;
byte btStatus=0; // 0 - non initialized; 1 - initialized; 2 - connected; 3 - error

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
  DEBUG_PRINTLN();
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
