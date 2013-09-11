#include <Arduino.h>
void MoveBackward(byte powerPercent, unsigned int distanceCm=0, boolean delayWhileMoving=false);
void MoveForward(byte powerPercent, unsigned int distanceCm=0, boolean delayWhileMoving=false);
void MoveWheels(byte wheelDirections, byte powerPercentTL, byte powerPercentTR, byte powerPercentBR, byte powerPercentBL);
void TurnLeft(byte powerPercent, unsigned int deltaDegrees=0, boolean delayWhileMoving=false);
boolean SetMode(byte newMode);
void InitIRSensor();
