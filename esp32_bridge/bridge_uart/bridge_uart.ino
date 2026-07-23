#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UART to STM32
HardwareSerial STM32Serial(2);

#define TXD 17
#define RXD 16

#define UART_FRAME_MAGIC_1 0xA5
#define UART_FRAME_MAGIC_2 0x5A
#define UART_FRAME_MAX_PAYLOAD 32
#define UART_FRAME_OVERHEAD 6

// Nordic UART Service UUIDs
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *txCharacteristic;

static uint16_t crc16CcittUpdate(uint16_t crc, uint8_t byte)
{
	crc ^= static_cast<uint16_t>(byte) << 8;
	for (uint8_t bit = 0; bit < 8; bit++)
	{
		crc = (crc & 0x8000U) != 0U
				  ? static_cast<uint16_t>((crc << 1) ^ 0x1021U)
				  : static_cast<uint16_t>(crc << 1);
	}
	return crc;
}

static bool writeUartFrame(const uint8_t *payload, size_t length)
{
	if (payload == nullptr || length == 0 || length > UART_FRAME_MAX_PAYLOAD)
	{
		return false;
	}

	uint8_t frame[UART_FRAME_MAX_PAYLOAD + UART_FRAME_OVERHEAD];
	uint16_t crc = 0xFFFFU;

	frame[0] = UART_FRAME_MAGIC_1;
	frame[1] = UART_FRAME_MAGIC_2;
	frame[2] = static_cast<uint8_t>(length & 0xFFU);
	frame[3] = static_cast<uint8_t>((length >> 8) & 0xFFU);
	crc = crc16CcittUpdate(crc, frame[2]);
	crc = crc16CcittUpdate(crc, frame[3]);

	for (size_t i = 0; i < length; i++)
	{
		frame[4 + i] = payload[i];
		crc = crc16CcittUpdate(crc, payload[i]);
	}

	frame[4 + length] = static_cast<uint8_t>(crc & 0xFFU);
	frame[5 + length] = static_cast<uint8_t>((crc >> 8) & 0xFFU);
	return STM32Serial.write(frame, length + UART_FRAME_OVERHEAD) ==
		   length + UART_FRAME_OVERHEAD;
}

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
			if (!writeUartFrame(
					reinterpret_cast<const uint8_t *>(data.c_str()),
					data.length()))
			{
				Serial.println("UART frame rejected or incomplete");
			}
		}
	}
};

void setup()
{
	Serial.begin(115200);

	// UART to STM32
	STM32Serial.begin(115200, SERIAL_8N1, RXD, TXD);

	// Initialize BLE
	BLEDevice::init("ClimateClock");

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
