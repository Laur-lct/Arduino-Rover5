//all compass stuff here

void InitCompass(){
  Wire.begin();
  delay(100);
  my3IMU.init(true); // the parameter enable or disable fast mode
  delay(100);
  isCompassEnabled=true;
  DEBUG_PRINTLN("Compass Initialized");
}
void DeactivateCompass(){
  isCompassEnabled=false;
}

float CalculateRadians(float* val){
   // Calculate heading when the magnetometer is level, then correct for signs of axis.
   float heading = atan2(val[7], val[6]);
   return heading;
}

float CalculateRadiansCompensated(float* val){
  // We are swapping the accelerometers axis as they are opposite to the compass the way we have them mounted.
  // We are swapping the signs axis as they are opposite.
  // Configure this for your setup.
  float accX = -val[0];
  float accY = val[1];
  
  float rollRadians = asin(accY);
  float pitchRadians = asin(accX);
  
  // We cannot correct for tilt over 40 degrees with this algorthem, if the board is tilted as such, return 0.
  if(rollRadians > 0.78 || rollRadians < -0.78 || pitchRadians > 0.78 || pitchRadians < -0.78)
    return 0;
  
  // Some of these are used twice, so rather than computing them twice in the algorithem we precompute them before hand.
  float cosRoll = cos(rollRadians);
  float sinRoll = sin(rollRadians);  
  float cosPitch = cos(pitchRadians);
  float sinPitch = sin(pitchRadians);
  
  // The tilt compensation algorithem.
  float Xh = val[6] * cosPitch + val[8] * sinPitch;
  float Yh = val[6] * sinRoll * sinPitch + val[7] * cosRoll - val[8] * sinRoll * cosPitch;
  return  atan2(Yh, Xh);
}

float RadiansToDegrees(float rads){
  if(rads < 0)
    rads += 2*PI;
  else if(rads > 2*PI)
    rads -= 2*PI;
  return rads * 180/PI;
}

float MeasureAngle(/*byte triesLeft=3*/){
  float raws[7];
  const float maxDiff = 0.02f;
  // the idea of filtering algorithm well explained here:   http://habrahabr.ru/post/167177/
  char candidateIndex=-1;
  char confidence=0;
  char lastMeasureIdx=6;
  for (char i=0; i<7; i++){
    my3IMU.getValues(rawCompassVals);
    raws[i] = CalculateRadiansCompensated(rawCompassVals);
    if (confidence == 0) {
      candidateIndex = i;
      confidence++;
    }
    else if (raws[candidateIndex]-raws[i] > -maxDiff && raws[candidateIndex]-raws[i] < maxDiff) 
      confidence++;
    else 
      confidence--;
    if (confidence> 7/2){
      lastMeasureIdx=i;
      break;
    }
    //delay(1);
  }
  float total=0;
  confidence=0;
  //DBG_ONLY(Serial.print("Compass raws: ")); 
  for (char i=0; i<=lastMeasureIdx; i++){
    //DBG_ONLY(Serial.print(raws[i]));
    //DBG_ONLY(Serial.print(" "));
    if (raws[candidateIndex]-raws[i] > -maxDiff && raws[candidateIndex]-raws[i] < maxDiff){
      confidence++;
      total+=raws[i];
    }
  }
  //DBG_ONLY(Serial.println());  
  //if (confidence > 7/2 || triesLeft==0)
    return RadiansToDegrees(total/confidence);
  //return MeasureAngle(--triesLeft);
}
