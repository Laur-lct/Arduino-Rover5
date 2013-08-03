// all servo stuff goes here

Servo panServo;
Servo tiltServo;
char currentPan = 0;
char currentTilt = 0;

void EnableHeadServos(){
  panServo.attach(PO_SEVRVO_HEAD_PAN);
  tiltServo.attach(PO_SEVRVO_HEAD_TILT);
}

void DisaleHeadServos(){
  panServo.detach();
  tiltServo.detach();
}
// sets head position to center
void CenterHead(){
  SetHeadPos(0,0);
}

void SetHeadPos(char panAngle, char tiltAngle){
  SetHeadPan(panAngle);
  SetHeadTilt(tiltAngle);
}

//angle relative to the center '-' = left '+' = right. 
void SetHeadPan(char panAngle){
 int realPan = HEAD_PAN_CENTER - panAngle;
 if (HEAD_PAN_CENTER - panAngle < HEAD_PAN_MIN) realPan = HEAD_PAN_MIN;
 else if (HEAD_PAN_CENTER - panAngle > HEAD_PAN_MAX) realPan = HEAD_PAN_MAX;
 if (!panServo.attached()) panServo.attach(PO_SEVRVO_HEAD_PAN);
 panServo.write(realPan);
 currentPan = (char)(HEAD_PAN_CENTER - realPan);
}
//angle relative to the center '-' = up '+' = down.
void SetHeadTilt(char tiltAngle){
 int realTilt = HEAD_TILT_CENTER + tiltAngle;
 if (realTilt > HEAD_TILT_MAX) realTilt = HEAD_TILT_MAX;
 else if (realTilt < HEAD_TILT_MIN) realTilt = HEAD_TILT_MIN;
 if (!tiltServo.attached()) tiltServo.attach(PO_SEVRVO_HEAD_TILT);
 tiltServo.write(realTilt);
 currentTilt = (char)(realTilt - HEAD_TILT_CENTER);
}
