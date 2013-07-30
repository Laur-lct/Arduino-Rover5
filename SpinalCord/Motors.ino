#include <TimerOne.h>

boolean isMoving = 0;
byte currentPowerTL = 0;
byte currentPowerTR = 0;
byte currentPowerBL = 0;
byte currentPowerBR = 0;

boolean encTL_prevState=0;
boolean encTR_prevState=0;
boolean encBL_prevState=0;
boolean encBR_prevState=0;
boolean isFwdTL=1;
boolean isFwdTR=1;
boolean isFwdBL=1;
boolean isFwdBR=1;

long totalPathOnTL=0;
long totalPathOnTR=0;
long totalPathOnBL=0;
long totalPathOnBR=0;

void InitEncoders(){
  // set a timer of length 1000 microseconds (or 0.001 sec - or 1 kHz)
  Timer1.initialize(1000);
  Timer1.attachInterrupt(TimerInterruptHandler); // attach the service routine here
  //Timer1.start();
}

void TimerInterruptHandler() {
  if (encTL_prevState != digitalRead(PI_MOTOR_ENC_TL)) {
    encTL_prevState=!encTL_prevState;
    if (isFwdTL) totalPathOnTL++;
    else totalPathOnTL--;
  }
  if (encTR_prevState != digitalRead(PI_MOTOR_ENC_TR)) {
    encTR_prevState=!encTR_prevState;
    if (isFwdTR) totalPathOnTR++;
    else totalPathOnTR--;
  }
  if (encBL_prevState != digitalRead(PI_MOTOR_ENC_BL)) {
    encBL_prevState=!encBL_prevState;
    if (isFwdBL) totalPathOnBL++;
    else totalPathOnBL--;
  }
  if (encBR_prevState != digitalRead(PI_MOTOR_ENC_BR)) {
    encBR_prevState=!encBR_prevState;
    if (isFwdBR) totalPathOnBR++;
    else totalPathOnBR--;
  }
  
  //TODO: caibration here, but it has to be very fast!
  //CalibrateMotors();
}

void DeactivateEncoders(){
  Timer1.detachInterrupt();
  Timer1.stop();
}

void MoveWheels(byte wheels, byte moveSpeed, boolean directionFwd=1) {
  if (wheels & MOTOR_WHEEL_TL) {
    currentPowerTL = moveSpeed;
    isFwdTL = directionFwd;
    digitalWrite(PO_MOTOR_DIR_TL,!directionFwd);
    analogWrite(PP_MOTOR_SPD_TL,moveSpeed);
  }
  if (wheels & MOTOR_WHEEL_TR) {
    currentPowerTR = moveSpeed;
    isFwdTR = directionFwd;
    digitalWrite(PO_MOTOR_DIR_TR,!directionFwd);
    analogWrite(PP_MOTOR_SPD_TR,moveSpeed);
  }
  if (wheels & MOTOR_WHEEL_BL) {
    currentPowerBL = moveSpeed;
    isFwdBL = directionFwd;
    digitalWrite(PO_MOTOR_DIR_BL,directionFwd);
    analogWrite(PP_MOTOR_SPD_BL,moveSpeed);
  }
  if (wheels & MOTOR_WHEEL_BR) {
    currentPowerBR = moveSpeed;
    isFwdBR = directionFwd;
    digitalWrite(PO_MOTOR_DIR_BR,directionFwd);
    analogWrite(PP_MOTOR_SPD_BR,moveSpeed);
  }
  
  if (currentPowerTL > 0 || currentPowerTR > 0 || currentPowerBL > 0 || currentPowerBR > 0)
    InitEncoders();
  else 
    DeactivateEncoders();
}

void TurnLeft(byte moveSpeed) {
  MoveWheels(MOTOR_WHEEL_TL | MOTOR_WHEEL_BL, moveSpeed, false);
  MoveWheels(MOTOR_WHEEL_TR | MOTOR_WHEEL_BR, moveSpeed, true);
}

void TurnRight(int moveSpeed) {
  MoveWheels(MOTOR_WHEEL_TL | MOTOR_WHEEL_BL, moveSpeed, true);
  MoveWheels(MOTOR_WHEEL_TR | MOTOR_WHEEL_BR, moveSpeed, false);
}

void MovingTurnLeft (byte turnIntencity) {
  if (isMoving) {
  int moveSpeedTL = currentPowerTL - turnIntencity;
  int moveSpeedBL = currentPowerBL - turnIntencity;
  analogWrite(PP_MOTOR_SPD_TL,moveSpeedTL);
  analogWrite(PP_MOTOR_SPD_BL,moveSpeedBL);
  }
}

void MovingTurnRight (byte turnIntencity) {
  if (isMoving) {
  int moveSpeedTR = currentPowerTR - turnIntencity;
  int moveSpeedBR = currentPowerBR - turnIntencity;
  analogWrite(PP_MOTOR_SPD_TR,moveSpeedTR);
  analogWrite(PP_MOTOR_SPD_BR,moveSpeedBR);
  }
}
  
void StopMoving() {
  isMoving = 0;
  MoveWheels(MOTOR_WHEEL_TL | MOTOR_WHEEL_TR | MOTOR_WHEEL_BL | MOTOR_WHEEL_BR, 0);
}

