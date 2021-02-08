#define USE_AM2320_DHT

#include <nRF5x_BLE_API.h>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  BLE Properties
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

BLEDevice ble;
const int RX_BUF_LEN = 8;

// UUID of service and characteristics
static const uint8_t service1_uuid[]        = {0x44, 0x78, 0x9f, 0x46, 0xbb, 0xd6, 0x45, 0xe3, 0x94, 0x59, 0x21, 0xa5, 0x59, 0x71, 0x65, 0x61};
static const uint8_t characteristic1_uuid[] = {0x68, 0x2e, 0xf6, 0xf6, 0xca, 0x59, 0x48, 0xe3, 0x9f, 0x50, 0x01, 0x6d, 0xc3, 0xc3, 0x87, 0x01};

uint8_t rx_value[RX_BUF_LEN] = {0, };

GattCharacteristic characteristic1(characteristic1_uuid, rx_value, 1, RX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);
GattCharacteristic *testChars[] = {&characteristic1};
GattService testService(service1_uuid, testChars, sizeof(testChars) / sizeof(GattCharacteristic *));


void connectionCallBack(const Gap::ConnectionCallbackParams_t *params) {
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_DISABLE);
  Serial.begin(9600);
  Serial.println("connected");
}

void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
  Serial.end();
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
  ble.startAdvertising();
}

void gattServerReadAuthCallBack(GattReadAuthCallbackParams *Handler) {
  // do nothing
}

void setup() {
  // put your setup code here, to run once
  // close peripheral power
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);

  ble.init();
  ble.onConnection(connectionCallBack);
  ble.onDisconnection(disconnectionCallBack);
  characteristic1.setReadAuthorizationCallback(gattServerReadAuthCallBack);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME, (const uint8_t *)"ble_osada", sizeof("ble_osada") - 1);
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.addService(testService);
  ble.setDeviceName((const uint8_t *)"internet-of-wall");
  ble.setTxPower(0);
  ble.setAdvertisingInterval(160);
  ble.setAdvertisingTimeout(0);
  ble.startAdvertising();
}

void loop() {
  ble.waitForEvent();
}
