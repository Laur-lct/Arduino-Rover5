//all bluetooth related functions will go here

// Hi everybody :) i hope ... and hope dies last..

void InitBluetooth(){
  
  Serial2.begin(115200);  // SoftwareSerial "com port" data rate. JY-MCU v1.03 defaults to 9600.
}

// check if BT is available
bool CheckIfBTisAvail(){
  return Serial2.available();
}

// read device output if available
String ReadDeviceOutput(){
  String output = "";
  if ( CheckIfBTisAvail() ) {
    while(Serial2.available()) { // While there is more to be read, keep reading.
      output += (char)Serial2.read();
    }
    //Serial.println(output);
  }
  return output;
}

// for BT programming purposes
void WriteToBT(){
  // Read user input if available.
  if (Serial2.available()){
    delay(10); // The DELAY!
    Serial2.write(Serial.read());
  }
}

