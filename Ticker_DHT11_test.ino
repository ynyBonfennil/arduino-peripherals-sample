
#include <DHT.h>

Ticker ticker1;

const int PIN_DHT = D2;

//DHT dht(PIN_DHT, DHT11);
DHT dht(PIN_DHT, DHT22);    // for AM2320
//DHT dht(PIN_DHT, DHT21);

void sensing_task_handle(void) {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.print("Humidity:  ");
  Serial.print(humidity);
  Serial.println("%\t");
  Serial.print("Temperature:  ");
  Serial.print(temperature);
  Serial.println(" *C");
}



void setup() {
  // Serial
  Serial.begin(9600);

  // dht
  dht.begin();

  // Power
  pinMode(D4, OUTPUT);

  // Ticker 1
  ticker1.attach(sensing_task_handle, 2);
  
}

void loop() {
  
}
