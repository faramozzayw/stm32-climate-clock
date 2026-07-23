#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "uart_framing.h"

// UART to STM32
HardwareSerial STM32Serial(2);

#define TXD 17
#define RXD 16

// Nordic UART Service UUIDs
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *txCharacteristic;
uart_frame_parser_t stm32FrameParser;

static bool writeUartFrame(const uint8_t *payload, size_t length)
{
	uint8_t frame[UART_FRAME_MAX_SIZE];
	uint16_t frameLength;

	if (length > UART_FRAME_MAX_PAYLOAD_SIZE)
	{
		return false;
	}

	if (!uart_frame_encode(
			payload,
			static_cast<uint16_t>(length),
			frame,
			sizeof(frame),
			&frameLength))
	{
		return false;
	}

	return STM32Serial.write(frame, frameLength) == frameLength;
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
	uart_frame_parser_init(&stm32FrameParser);

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
	while (STM32Serial.available())
	{
		uart_frame_view_t frame;
		uart_frame_result_t result = uart_frame_parser_process(
			&stm32FrameParser,
			static_cast<uint8_t>(STM32Serial.read()),
			&frame);

		if (result == UART_FRAME_RESULT_COMPLETE)
		{
			txCharacteristic->setValue(frame.payload, frame.payload_length);
			txCharacteristic->notify();
		}
		else if (result == UART_FRAME_RESULT_ERROR)
		{
			Serial.println("Invalid UART frame received from STM32");
		}
	}

	delay(1);
}
