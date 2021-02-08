
#include <nRF5x_BLE_API.h>
#include <Wire.h>

#define DEVICE_NAME   "BLE_Sample_osada"
#define RX_BUF_LEN  20

// ---------------------------------------------------------
//  BLE Properties
// ---------------------------------------------------------
BLE                   ble;

static uint8_t rx_buf[RX_BUF_LEN];
static uint8_t rx_buf_num;

// UUID of service and characteristics
static const uint8_t service1_uuid[]        = {0x44, 0x78, 0x9f, 0x46, 0xbb, 0xd6, 0x45, 0xe3, 0x94, 0x59, 0x21, 0xa5, 0x59, 0x71, 0x65, 0x61};
static const uint8_t characteristic1_uuid[] = {0x68, 0x2e, 0xf6, 0xf6, 0xca, 0x59, 0x48, 0xe3, 0x9f, 0x50, 0x01, 0x6d, 0xc3, 0xc3, 0x87, 0x01};

uint8_t rx_value[RX_BUF_LEN] = {0, };

GattCharacteristic characteristic1(characteristic1_uuid, rx_value, 1, RX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
GattCharacteristic *testChars[] = {&characteristic1};
GattService testService(service1_uuid, testChars, sizeof(testChars) / sizeof(GattCharacteristic *));


void connectionCallBack(const Gap::ConnectionCallbackParams_t *params) {
  Serial.println("Connected");
}

void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
  Serial.println("Disconnected");
  ble.startAdvertising();
}

// ------
// Sensing
// ------

#define DEV_ADR 0x5c  // AM2320スレーブアドレス
uint8_t data[8];

// This IRQ handler will trigger every 1000us
extern "C" {
  void TIMER2_IRQHandler(void) {
    digitalWrite(D4, HIGH);

    Serial.println("m_ticker1_handle");

    // AM2320センサーのウェイクアップ
    Wire.beginTransmission(DEV_ADR);
    Serial.println("s");
    Wire.endTransmission();

    Serial.println("A");

    // 湿度・温度データ取得要求
    Wire.beginTransmission(DEV_ADR);
    Wire.write(0x03);         // レジスタ読み取りコマンド
    Wire.write(0x00);         // 読み取り開始レジスタ番号
    Wire.write(0x04);         // 読み取りデータ数
    Wire.endTransmission();

    Serial.println("B");

    // データの取得
    Wire.requestFrom(DEV_ADR, 8);
    if (Wire.available() >= 8) {
      for (uint8_t i = 0; i < 8; i++) {
        data[i] = Wire.read();
      }

      float rh = ((float)(data[2] * 256 + data[3])) / 10; // 湿度
      float tp = ((float)(data[4] * 256 + data[5])) / 10; // 温度

      // 湿度・温度の表示
      Serial.print("T=");
      Serial.print(tp);
      Serial.print("c");
      Serial.print(" H=");
      Serial.print(rh);
      Serial.println("%");
    }

    Serial.println("m_ticker1_handle end");
    digitalWrite(D4, LOW);
  }
}




// ---------------------------------------------------------
//  Main Routine
// ---------------------------------------------------------
void setup() {
  // Serial
  Serial.begin(9600);

  // Wire (I2C)
  Wire.begin();

  // Power
  pinMode(D4, OUTPUT);

  // Timer1
  Serial.begin(9600);
  NRF_TIMER2->MODE        = 0UL;
  NRF_TIMER2->BITMODE     = 3UL;
  NRF_TIMER2->PRESCALER = 9UL;
  NRF_TIMER2->CC[0] = 32000UL;
  NRF_TIMER2->INTENSET = (unsigned long)(1 << 16); // Compare with 0
  NRF_TIMER2->SHORTS = 1;  // Clear the timer when COMPARE0 event is triggered

  __sd_nvic_irq_enable();
  sd_nvic_SetPriority(TIMER2_IRQn, 3);
  uint32_t error_ret = sd_nvic_EnableIRQ(TIMER2_IRQn);

  NRF_TIMER2->TASKS_CLEAR = 1;               // clear the task first to be usable for later
  NRF_TIMER2->TASKS_START = 1UL;

  // BLE
  ble.init();
  ble.onConnection(connectionCallBack);
  ble.onDisconnection(disconnectionCallBack);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME, (const uint8_t *)"ble_osada", sizeof("ble_osada") - 1);
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.addService(testService);
  ble.setDeviceName((const uint8_t *)DEVICE_NAME);
  ble.setTxPower(-40);
  ble.setAdvertisingInterval(160);
  ble.setAdvertisingTimeout(0);
  ble.startAdvertising();
}

void loop() {
  ble.waitForEvent();
}
