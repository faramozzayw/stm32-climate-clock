#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UART to STM32
HardwareSerial STM32Serial(2);

#define TXD 17
#define RXD 16

// Nordic UART Service UUIDs
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *txCharacteristic;

class RxCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic) override
  {
    String data = characteristic->getValue();

    Serial.print("BLE received ");
    Serial.print(data.length());
    Serial.print(" bytes: ");

    for (size_t i = 0; i < data.length(); i++)
    {
      Serial.printf("%02X ", static_cast<uint8_t>(data[i]));
    }

    Serial.println();

    if (data.length() > 0)
    {
      STM32Serial.write(
          reinterpret_cast<const uint8_t *>(data.c_str()),
          data.length());
    }
  }
};

void setup()
{
  Serial.begin(115200);

  // UART to STM32
  STM32Serial.begin(115200, SERIAL_8N1, RXD, TXD);

  // Initialize BLE
  BLEDevice::init("ESP32_BRIDGE");

  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(SERVICE_UUID);

  // Phone -> ESP32 -> STM32
  BLECharacteristic *rxCharacteristic = service->createCharacteristic(
      CHARACTERISTIC_RX_UUID,
      BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_WRITE_NR);

  rxCharacteristic->setCallbacks(new RxCallbacks());

  // Optional notification characteristic for STM32 -> phone
  txCharacteristic = service->createCharacteristic(
      CHARACTERISTIC_TX_UUID,
      BLECharacteristic::PROPERTY_NOTIFY);

  txCharacteristic->addDescriptor(new BLE2902());

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("BLE bridge is advertising");
}

void loop()
{
  // STM32 -> ESP32 -> phone
  if (STM32Serial.available())
  {
    uint8_t buffer[128];
    size_t length = 0;

    while (STM32Serial.available() && length < sizeof(buffer))
    {
      buffer[length++] = STM32Serial.read();
    }

    txCharacteristic->setValue(buffer, length);
    txCharacteristic->notify();
  }

  delay(1);
}
