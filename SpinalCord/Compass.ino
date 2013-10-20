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

float CalculateHeadingNotTiltCompensated(float* val)
{
   // Calculate heading when the magnetometer is level, then correct for signs of axis.
   float heading = atan2(val[7], val[6]);
   return heading;
}

float CalculateHeadingTiltCompensated(float* val)
{
  // We are swapping the accelerometers axis as they are opposite to the compass the way we have them mounted.
  // We are swapping the signs axis as they are opposite.
  // Configure this for your setup.
  float accX = -val[0];
  float accY = val[1];
  
  float rollRadians = asin(accY);
  float pitchRadians = asin(accX);
  
  // We cannot correct for tilt over 40 degrees with this algorthem, if the board is tilted as such, return 0.
  if(rollRadians > 0.78 || rollRadians < -0.78 || pitchRadians > 0.78 || pitchRadians < -0.78)
  {
    return 0;
  }
  
  // Some of these are used twice, so rather than computing them twice in the algorithem we precompute them before hand.
  float cosRoll = cos(rollRadians);
  float sinRoll = sin(rollRadians);  
  float cosPitch = cos(pitchRadians);
  float sinPitch = sin(pitchRadians);
  
  // The tilt compensation algorithem.
  float Xh = val[6] * cosPitch + val[8] * sinPitch;
  float Yh = val[6] * sinRoll * sinPitch + val[7] * cosRoll - val[8] * sinRoll * cosPitch;
  
  float heading = atan2(Yh, Xh);
    
  return heading;
}

float RadiansToDegrees(float rads)
{
  // Correct for when signs are reversed.
  if(rads < 0)
    rads += 2*PI;
      
  // Check for wrap due to addition of declination.
  if(rads > 2*PI)
    rads -= 2*PI;
   
  // Convert radians to degrees for readability.
  float heading = rads * 180/PI;
  DBG_ONLY(Serial.print("Compass degrees = "));
  DBG_ONLY(Serial.println(heading));
  return heading;
}
