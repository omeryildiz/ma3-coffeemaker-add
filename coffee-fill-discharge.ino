#define DEBUG_MOD 0

#if DEBUG_MOD == 1
#define debug(x)(Serial.print(x))
#define debugln(x) (Serial.println(x))
#else
#define debug(x)
#define debugln(x)
#endif


#define SENSOR1_TH 350
#define SENSOR2_TH 550
#define WATER_LEVEL_SENSOR1 A1
#define WATER_LEVEL_SENSOR2 A2
#define MOTORDRIVER_DISCHARGE_WATER_1  2
#define MOTORDRIVER_DISCHARGE_WATER_2  4
#define MOTORDRIVER_FILL_WATER_3  6
#define MOTORDRIVER_FILL_WATER_4  7
#define HCSR04_PIN_TRIG 9
#define HCSR04_PIN_ECHO 8
#define BUZZER 3
#define DISTANCE_SENSOR_TUNNER_COUNT 5

const unsigned long TEN_SECOND_IN_MILLISECOND = (10UL * 1000 * 1);
const unsigned long MINUTE_IN_MILLISECOND = (60UL * 1000 * 1);
const unsigned long RUN_DURATION_LIMIT = (1 * TEN_SECOND_IN_MILLISECOND);
const float WATER_LIMIT_LEVEL = 3.0;
const float VOICE_SPEED_AIR = 0.034 / 2;
const float VOICE_SPEED_WATER = 0.1480 / 2;

bool engineFlag = false;
char printBuffer[128];
unsigned long currentTime = 0;
unsigned long elapsedTime = 0;
unsigned long startTime = 0;




void setup() {

  Serial.begin (9600);

  pinMode(MOTORDRIVER_DISCHARGE_WATER_1, OUTPUT);
  pinMode(MOTORDRIVER_DISCHARGE_WATER_2, OUTPUT);
  pinMode(MOTORDRIVER_FILL_WATER_3, OUTPUT);
  pinMode(MOTORDRIVER_FILL_WATER_4, OUTPUT);

  pinMode(HCSR04_PIN_TRIG, OUTPUT);
  pinMode(HCSR04_PIN_ECHO, INPUT);

  pinMode(BUZZER, OUTPUT);

  digitalWrite(MOTORDRIVER_DISCHARGE_WATER_2, LOW);
  digitalWrite(MOTORDRIVER_FILL_WATER_4, LOW);

}

void loop() {
  bottleFullWarning();
  waterDischarger();
  waterFiller();
  delay(2000);

}

float readWaterLevel() {


  static float distance = 0.0;
  unsigned long duration = 0UL;

  duration = 0UL;
  digitalWrite(HCSR04_PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(HCSR04_PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(HCSR04_PIN_TRIG, LOW);
  duration = pulseIn(HCSR04_PIN_ECHO, HIGH);
  distance = duration * VOICE_SPEED_AIR;
  debugln(distance);

  debug("Distance calculation result:");
  debugln(distance);
  return distance;
}

void waterFiller() {

  float waterLevel = 0;
  for (int i = 0 ; i < DISTANCE_SENSOR_TUNNER_COUNT ; i++ ) {
    waterLevel += readWaterLevel();
  }
  waterLevel = waterLevel / DISTANCE_SENSOR_TUNNER_COUNT;
  debug("waterlevel:");
  debugln(waterLevel);

  if (waterLevel < WATER_LIMIT_LEVEL ) {
    runMotorCw(MOTORDRIVER_FILL_WATER_3, MOTORDRIVER_FILL_WATER_4);
    debugln("water filler motor is on");
    delay(1000);
  } else {
    stopMotor(MOTORDRIVER_FILL_WATER_3, MOTORDRIVER_FILL_WATER_4);
    debugln("water filler motor is off");
  }

}

void bottleFullWarning() {
  int sensor2 = analogRead(WATER_LEVEL_SENSOR2);
  debug("ADC2 level is ");
  debugln(sensor2);
  while (sensor2 >= SENSOR2_TH) {
    sensor2 = analogRead(WATER_LEVEL_SENSOR2);
    debug("ADC2 level is ");
    debugln(sensor2);
    beepSound();
  }
}

void beepSound() {
  tone(BUZZER, 1000);
  delay(1000);
  noTone(BUZZER);
  delay(1000);
}

void waterDischarger() {
  int sensor1 = analogRead(WATER_LEVEL_SENSOR1);
  sprintf(printBuffer, "ADC1 level is %d", sensor1);
  debugln(printBuffer);
  if (sensor1 >= SENSOR1_TH) {
    if (engineFlag == false) {
      if (elapsedTime > RUN_DURATION_LIMIT) {
        stopMotor(MOTORDRIVER_DISCHARGE_WATER_1, MOTORDRIVER_DISCHARGE_WATER_2);
        debugln("engine is off");
        elapsedTime = 0;
        engineFlag = false;
      } else {
        runMotorCw(MOTORDRIVER_DISCHARGE_WATER_1, MOTORDRIVER_DISCHARGE_WATER_2);
        debugln("engine is on");
        startTime = millis();
        engineFlag = true;
      }
    } else {
      if (elapsedTime > RUN_DURATION_LIMIT) {
        stopMotor(MOTORDRIVER_DISCHARGE_WATER_1, MOTORDRIVER_DISCHARGE_WATER_2);
        Serial.println("engine is off");
        elapsedTime = 0;
        engineFlag = false;
        delay(MINUTE_IN_MILLISECOND);
      } else {
        runMotorCw(MOTORDRIVER_DISCHARGE_WATER_1, MOTORDRIVER_DISCHARGE_WATER_2);
        debugln("engine is on");
        currentTime = millis();
        elapsedTime = currentTime - startTime;
      }

    }


  } else {
    //engineFlag = false;
    stopMotor(MOTORDRIVER_DISCHARGE_WATER_1, MOTORDRIVER_DISCHARGE_WATER_2);
    debugln("engine is off");
    elapsedTime = 0;
    engineFlag = false;
  }
}

void runMotorCw(int pin1, int pin2) {
  digitalWrite(pin1, HIGH);
}

void stopMotor(int pin1, int pin2) {
  digitalWrite(pin1, LOW);
}
