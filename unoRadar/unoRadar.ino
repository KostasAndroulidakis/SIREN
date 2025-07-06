void setup() {
  Serial.begin(9600);
  initMotor();
  initSensor();
}

void loop() {
  // Sweep forward
  for(int angle = getMinAngle(); angle <= getMaxAngle(); angle++) {
    moveServoToAngle(angle);
    int distance = getDistance();
    
    Serial.print(angle);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
  }
  
  // Sweep backward
  for(int angle = getMaxAngle(); angle >= getMinAngle(); angle--) {
    moveServoToAngle(angle);
    int distance = getDistance();
    
    Serial.print(angle);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
  }
}