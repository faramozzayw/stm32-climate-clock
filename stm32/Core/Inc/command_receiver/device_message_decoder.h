#ifndef INC_DEVICE_MESSAGE_DECODER_H_
#define INC_DEVICE_MESSAGE_DECODER_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	DEVICE_MESSAGE_NONE = 0,
	DEVICE_MESSAGE_SET_MAX_TEMP,
	DEVICE_MESSAGE_SET_MIN_TEMP,
	DEVICE_MESSAGE_SET_CURRENT_TIME,
	DEVICE_MESSAGE_SET_TEMPERATURE_UNIT,
	DEVICE_MESSAGE_BLE_CONNECTION_STATE,
} device_message_type_t;

typedef enum
{
	BLE_CONNECTION_STATE_DISCONNECTED = 0,
	BLE_CONNECTION_STATE_CONNECTING,
	BLE_CONNECTION_STATE_CONNECTED,
	BLE_CONNECTION_STATE_DISCONNECTING,
} ble_connection_state_t;

typedef enum
{
	DEVICE_TEMPERATURE_UNIT_CELSIUS = 0,
	DEVICE_TEMPERATURE_UNIT_FAHRENHEIT,
} device_temperature_unit_t;

typedef struct
{
	device_message_type_t type;
	union
	{
		int16_t temperature;
		uint64_t current_time_ms;
		device_temperature_unit_t temperature_unit;
		ble_connection_state_t ble_connection_state;
	} value;
} decoded_device_message_t;

typedef enum
{
	DEVICE_MESSAGE_DECODE_OK = 0,
	DEVICE_MESSAGE_DECODE_INVALID_ARGUMENT,
	DEVICE_MESSAGE_DECODE_INVALID_PROTOBUF,
	DEVICE_MESSAGE_DECODE_MISSING_PAYLOAD,
	DEVICE_MESSAGE_DECODE_MISSING_COMMAND,
	DEVICE_MESSAGE_DECODE_UNEXPECTED_PAYLOAD,
	DEVICE_MESSAGE_DECODE_VALUE_OUT_OF_RANGE,
} device_message_decode_result_t;

#ifdef __cplusplus
extern "C"
{
#endif

	device_message_decode_result_t device_message_decode(
		const uint8_t *payload,
		uint16_t payload_length,
		decoded_device_message_t *message);

#ifdef __cplusplus
}
#endif

#endif /* INC_DEVICE_MESSAGE_DECODER_H_ */
