const int TRIG_PIN = 2;
const int ECHO_PIN = 3;
const int MIN_DISTANCE = 2;
const int MAX_DISTANCE = 400;

void initSensor() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;
  
  return (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE) ? distance : 0;
}

bool isValidDistance(int distance) {
  return (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE);
}