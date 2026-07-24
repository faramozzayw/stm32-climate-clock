#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "device.pb.h"
#include "pb_encode.h"
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
volatile bool bleLinkConnected = false;
volatile bool bleNotificationsEnabled = false;

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

static bool writeBleConnectionStatus(device_BleConnectionState state)
{
	device_DeviceMessage message = device_DeviceMessage_init_zero;
	uint8_t payload[device_DeviceMessage_size];
	pb_ostream_t stream = pb_ostream_from_buffer(payload, sizeof(payload));

	message.which_payload = device_DeviceMessage_bridge_status_tag;
	message.payload.bridge_status.ble_connection_state = state;

	if (!pb_encode(&stream, device_DeviceMessage_fields, &message))
	{
		return false;
	}

	return writeUartFrame(payload, stream.bytes_written);
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

class TxDescriptorCallbacks : public BLEDescriptorCallbacks
{
	void onWrite(BLEDescriptor *descriptor) override
	{
		BLE2902 *configuration = static_cast<BLE2902 *>(descriptor);

		bleNotificationsEnabled = configuration->getNotifications();

		if (!bleLinkConnected)
		{
			return;
		}

		device_BleConnectionState state = bleNotificationsEnabled
											  ? device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTED
											  : device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTING;

		if (!writeBleConnectionStatus(state))
		{
			Serial.println("Failed to report BLE notification state");
		}
	}
};

class ServerCallbacks : public BLEServerCallbacks
{
	void onConnect(BLEServer *server) override
	{
		(void)server;
		bleLinkConnected = true;
		bleNotificationsEnabled = false;

		if (!writeBleConnectionStatus(
				device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTING))
		{
			Serial.println("Failed to report BLE connection attempt");
		}
	}

	void onDisconnect(BLEServer *server) override
	{
		bleLinkConnected = false;
		bleNotificationsEnabled = false;

		if (!writeBleConnectionStatus(
				device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTED))
		{
			Serial.println("Failed to report BLE disconnection");
		}

		server->startAdvertising();
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
	server->setCallbacks(new ServerCallbacks());
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

	BLE2902 *notificationDescriptor = new BLE2902();
	notificationDescriptor->setCallbacks(new TxDescriptorCallbacks());
	txCharacteristic->addDescriptor(notificationDescriptor);

	service->start();

	BLEAdvertising *advertising = BLEDevice::getAdvertising();
	advertising->addServiceUUID(SERVICE_UUID);
	advertising->setScanResponse(true);
	BLEDevice::startAdvertising();

	if (!writeBleConnectionStatus(
			device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTED))
	{
		Serial.println("Failed to report initial BLE state");
	}

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
			if (bleNotificationsEnabled)
			{
				txCharacteristic->setValue(frame.payload, frame.payload_length);
				txCharacteristic->notify();
			}
		}
		else if (result == UART_FRAME_RESULT_ERROR)
		{
			Serial.println("Invalid UART frame received from STM32");
		}
	}

	delay(1);
}
