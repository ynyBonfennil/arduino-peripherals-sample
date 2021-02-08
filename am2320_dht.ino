#ifdef USE_AM2320_DHT

#include <DHT.h>

// - - - - - - - - - - - - - - -
//  Constant Value
// - - - - - - - - - - - - - - -

const int PIN_SENSOR1_PWR = D5;
const int PIN_SENSOR2_PWR = D7;

const int PIN_SENSOR1_WIRE = D4;
const int PIN_SENSOR2_WIRE = D6;

DHT sensor1(PIN_SENSOR1_WIRE, DHT22);
DHT sensor2(PIN_SENSOR2_WIRE, DHT22);

// - - - - - - - - - - - - - - -
//  Public Function
// - - - - - - - - - - - - - - -

void sensor1_init() {
  pinMode(PIN_SENSOR1_PWR, OUTPUT);
  sensor1.begin();
}

void sensor2_init() {
  pinMode(PIN_SENSOR2_PWR, OUTPUT);
  sensor2.begin();
}

void sensor1_read(float *humidity, float *temperature) {
  digitalWrite(PIN_SENSOR1_PWR, HIGH);
  delay(500);
  
  *humidity = sensor1.readHumidity();
  *temperature = sensor1.readTemperature();

  //digitalWrite(PIN_SENSOR1_PWR, LOW);
}

void sensor2_read(float *humidity, float *temperature) {
  digitalWrite(PIN_SENSOR2_PWR, HIGH);
  delay(100);
  
  *humidity = sensor2.readHumidity();
  *temperature = sensor2.readTemperature();

  digitalWrite(PIN_SENSOR2_PWR, LOW);
}

#endif
