/**
 * FreeIMU library serial communication protocol
*/

void StartFreeimuCalib() {
  DeactivateCommonInterrupt();
  InitBluetooth(115200);
  InitCompass();
  statusLED1->on();
  statusLED2->on();
}

void writeArr(void * varr, uint8_t arr_length, uint8_t type_bytes) {
  byte * arr = (byte*) varr;
  for(uint8_t i=0; i<arr_length; i++) {
    writeVar(&arr[i * type_bytes], type_bytes);
  }
}

void writeVar(void * val, uint8_t type_bytes) {
  byte * addr=(byte *)(val);
  for(uint8_t i=0; i<type_bytes; i++) { 
    Serial2.write(addr[i]);
  }
}
void serialPrintFloatArr(float * arr, int length) {
  for(int i=0; i<length; i++) {
    serialFloatPrint(arr[i]);
    Serial2.print(",");
  }
}


void serialFloatPrint(float f) {
  byte * b = (byte *) &f;
  for(int i=0; i<4; i++) {
    byte b1 = (b[i] >> 4) & 0x0f;
    byte b2 = (b[i] & 0x0f);
    char c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
    char c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;
    Serial2.print(c1);
    Serial2.print(c2);
  }
}

void RunFreeimuCalib() {
  float q[4];
  int raw_values[9];
  float ypr[3]; // yaw pitch roll
  char str[256];
  float val[9];
  //The command from the PC
  char _pcCmd;
  
  if(Serial2.available()) {
    _pcCmd = Serial2.read();
    if(_pcCmd=='v') {
      sprintf(str, "FreeIMU library by %s, FREQ:%s, LIB_VERSION: %s, IMU: %s", FREEIMU_DEVELOPER, FREEIMU_FREQ, FREEIMU_LIB_VERSION, FREEIMU_ID);
      Serial2.print(str);
      Serial2.print('\n');
    }
    else if(_pcCmd=='r') {
      uint8_t count = serial_busy_wait();
      for(uint8_t i=0; i<count; i++) {
        my3IMU.getRawValues(raw_values);
        sprintf(str, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,", raw_values[0], raw_values[1], raw_values[2], raw_values[3], raw_values[4], raw_values[5], raw_values[6], raw_values[7], raw_values[8], raw_values[9], raw_values[10]);
        Serial2.print(str);
        Serial2.print('\n');
      }
    }
    else if(_pcCmd=='b') {
      uint8_t count = serial_busy_wait();
      for(uint8_t i=0; i<count; i++) {
        #if HAS_ITG3200()
          my3IMU.acc.readAccel(&raw_values[0], &raw_values[1], &raw_values[2]);
          my3IMU.gyro.readGyroRaw(&raw_values[3], &raw_values[4], &raw_values[5]);
        #else // MPU6050
          my3IMU.accgyro.getMotion6(&raw_values[0], &raw_values[1], &raw_values[2], &raw_values[3], &raw_values[4], &raw_values[5]);
        #endif
        writeArr(raw_values, 6, sizeof(int)); // writes accelerometer and gyro values
        #if IS_9DOM()
          my3IMU.magn.getValues(&raw_values[0], &raw_values[1], &raw_values[2]);
          writeArr(raw_values, 3, sizeof(int));
        #endif
        Serial2.println();
      }
    }
    else if(_pcCmd == 'q') {
      uint8_t count = serial_busy_wait();
      for(uint8_t i=0; i<count; i++) {
        my3IMU.getQ(q);
        serialPrintFloatArr(q, 4);
        Serial2.println("");
      }
    }
    #ifndef CALIBRATION_H
    else if(_pcCmd == 'c') {
      const uint8_t eepromsize = sizeof(float) * 6 + sizeof(int) * 6;
      while(Serial2.available() < eepromsize) ; // wait until all calibration data are received
      EEPROM.write(MEMADDR_FREEIMU_START, FREEIMU_EEPROM_SIGNATURE);
      for(uint8_t i = 1; i<(eepromsize + 1); i++) {
        EEPROM.write(MEMADDR_FREEIMU_START + i, (char) Serial2.read());
      }
      my3IMU.calLoad(); // reload calibration
    }
    #endif
    else if(_pcCmd == 'C') { // check calibration values
      Serial2.print("acc offset: ");
      Serial2.print(my3IMU.acc_off_x);
      Serial2.print(",");
      Serial2.print(my3IMU.acc_off_y);
      Serial2.print(",");
      Serial2.print(my3IMU.acc_off_z);
      Serial2.print("\n");
      
      Serial2.print("magn offset: ");
      Serial2.print(my3IMU.magn_off_x);
      Serial2.print(",");
      Serial2.print(my3IMU.magn_off_y);
      Serial2.print(",");
      Serial2.print(my3IMU.magn_off_z);
      Serial2.print("\n");
      
      Serial2.print("acc scale: ");
      Serial2.print(my3IMU.acc_scale_x);
      Serial2.print(",");
      Serial2.print(my3IMU.acc_scale_y);
      Serial2.print(",");
      Serial2.print(my3IMU.acc_scale_z);
      Serial2.print("\n");
      
      Serial2.print("magn scale: ");
      Serial2.print(my3IMU.magn_scale_x);
      Serial2.print(",");
      Serial2.print(my3IMU.magn_scale_y);
      Serial2.print(",");
      Serial2.print(my3IMU.magn_scale_z);
      Serial2.print("\n");
    }
    else if(_pcCmd == 'd') { // debugging outputs
      while(1) {
        my3IMU.getRawValues(raw_values);
        sprintf(str, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,", raw_values[0], raw_values[1], raw_values[2], raw_values[3], raw_values[4], raw_values[5], raw_values[6], raw_values[7], raw_values[8], raw_values[9], raw_values[10]);
        Serial2.print(str);
        Serial2.print('\n');
        my3IMU.getQ(q);
        serialPrintFloatArr(q, 4);
        Serial2.println("");
        my3IMU.getYawPitchRoll(ypr);
        Serial2.print("Yaw: ");
        Serial2.print(ypr[0]);
        Serial2.print(" Pitch: ");
        Serial2.print(ypr[1]);
        Serial2.print(" Roll: ");
        Serial2.print(ypr[2]);
        Serial2.println("");
      }
    }
    else if(_pcCmd == 'e') { // exit
      SetMode(prevMode);
    }
  }
}

char serial_busy_wait() {
  while(!Serial2.available()) {
    ; // do nothing until ready
  }
  return Serial2.read();
}

void FinishFreeimuCalib() {
  InitServiceInterrupt();
  statusLED1->off();
  statusLED2->off();
}
