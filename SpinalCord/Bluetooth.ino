//all bluetooth related functions will go here

// Hi everybody :) i hope ... and hope dies last..

SoftwareSerial bt_serial(0, 0);  // empty !!not valid till initialization

void InitBluetooth(){
  bt_serial = SoftwareSerial(PO_BLUETOOTH_TX, PI_BLUETOOTH_RX);  // set up pins for bluetooth
  Serial.begin(9600);    // Open serial communications and wait for port to open. @TODO check do we need it at all.. 
  bt_serial.begin(9600);  // SoftwareSerial "com port" data rate. JY-MCU v1.03 defaults to 9600.
}

// check if BT is available
bool CheckIfBTisAvail(){
  return bt_serial.available();
}

// read device output if available
String ReadDeviceOutput(){
  String output = "";
  if ( CheckIfBTisAvail() ) {
    while(bt_serial.available()) { // While there is more to be read, keep reading.
      output += (char)bt_serial.read();
    }
    //Serial.println(output);
  }
  return output;
}

// for BT programming purposes
void WriteToBT(){
  // Read user input if available.
  if (Serial.available()){
    delay(10); // The DELAY!
    bt_serial.write(Serial.read());
  }
}

