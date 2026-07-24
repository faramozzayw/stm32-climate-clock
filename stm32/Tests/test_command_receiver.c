#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "command_receiver/device_message_decoder.h"
#include "command_receiver/uart_command_receiver.h"
#include "command_receiver/uart_frame.h"
#include "device.pb.h"
#include "pb_encode.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define TEST_FRAME_MAX_SIZE (2U + 2U + UART_FRAME_MAX_PAYLOAD_SIZE + 2U)

#define CHECK(condition)                                                       \
	do                                                                         \
	{                                                                          \
		if (!(condition))                                                      \
		{                                                                      \
			printf("    CHECK failed at line %d: %s\n", __LINE__, #condition); \
			return false;                                                      \
		}                                                                      \
	} while (0)

static HAL_StatusTypeDef mock_receive_result = HAL_OK;
static uint32_t mock_receive_call_count;
static UART_HandleTypeDef *mock_receive_huart;
static uint8_t *mock_receive_data;
static uint16_t mock_receive_size;

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart,
	uint8_t *data, uint16_t size)
{
	mock_receive_call_count++;
	mock_receive_huart = huart;
	mock_receive_data = data;
	mock_receive_size = size;
	return mock_receive_result;
}

static void reset_uart_mock(void)
{
	mock_receive_result = HAL_OK;
	mock_receive_call_count = 0U;
	mock_receive_huart = NULL;
	mock_receive_data = NULL;
	mock_receive_size = 0U;
}

static uint16_t crc16_ccitt(const uint8_t *data, size_t length)
{
	uint16_t crc = 0xFFFFU;
	size_t pos;

	for (pos = 0U; pos < length; pos++)
	{
		uint8_t bit;

		crc ^= (uint16_t)data[pos] << 8U;
		for (bit = 0U; bit < 8U; bit++)
		{
			crc = (crc & 0x8000U) != 0U
					  ? (uint16_t)((crc << 1U) ^ 0x1021U)
					  : (uint16_t)(crc << 1U);
		}
	}

	return crc;
}

static size_t make_frame(const uint8_t *payload, size_t payload_length,
	uint8_t *frame)
{
	uint16_t crc;

	frame[0] = UART_FRAME_MAGIC_1;
	frame[1] = UART_FRAME_MAGIC_2;
	frame[2] = (uint8_t)payload_length;
	frame[3] = (uint8_t)(payload_length >> 8U);
	memcpy(&frame[4], payload, payload_length);
	crc = crc16_ccitt(&frame[2], payload_length + 2U);
	frame[4U + payload_length] = (uint8_t)crc;
	frame[5U + payload_length] = (uint8_t)(crc >> 8U);
	return payload_length + 6U;
}

static size_t encode_message(const device_DeviceMessage *message,
	uint8_t *payload)
{
	pb_ostream_t stream = pb_ostream_from_buffer(payload,
		UART_FRAME_MAX_PAYLOAD_SIZE);

	if (!pb_encode(&stream, device_DeviceMessage_fields, message))
	{
		return 0U;
	}

	return stream.bytes_written;
}

static size_t encode_command(const device_DeviceCommand *command,
	uint8_t *payload)
{
	device_DeviceMessage message = device_DeviceMessage_init_zero;

	message.which_payload = device_DeviceMessage_command_tag;
	message.payload.command = *command;
	return encode_message(&message, payload);
}

static void receiver_feed(uart_command_receiver_t *receiver,
	const uint8_t *bytes, size_t length)
{
	size_t pos;

	for (pos = 0U; pos < length; pos++)
	{
		*uart_command_receiver_rx_byte_ptr(receiver) = bytes[pos];
		uart_command_receiver_on_rx_complete(receiver);
	}
}

static bool test_frame_parser_accepts_valid_frame(void)
{
	const uint8_t payload[] = {0x08U, 0x96U, 0x01U};
	uint8_t encoded[TEST_FRAME_MAX_SIZE];
	size_t encoded_length = make_frame(payload, sizeof(payload), encoded);
	uart_frame_parser_t parser;
	uart_frame_view_t view = {0};
	size_t pos;

	uart_frame_parser_init(&parser);
	for (pos = 0U; pos + 1U < encoded_length; pos++)
	{
		CHECK(uart_frame_parser_process(&parser, encoded[pos], &view) ==
			  UART_FRAME_RESULT_IN_PROGRESS);
	}
	CHECK(uart_frame_parser_process(&parser, encoded[pos], &view) ==
		  UART_FRAME_RESULT_COMPLETE);
	CHECK(view.payload_length == sizeof(payload));
	CHECK(memcmp(view.payload, payload, sizeof(payload)) == 0);
	CHECK(parser.state == UART_FRAME_WAIT_MAGIC_1);
	return true;
}

static bool test_frame_parser_resynchronizes_after_noise_and_repeated_magic(void)
{
	const uint8_t payload[] = {0x01U};
	const uint8_t prefix[] = {0x00U, UART_FRAME_MAGIC_1, 0x00U,
		UART_FRAME_MAGIC_1};
	uint8_t encoded[TEST_FRAME_MAX_SIZE];
	size_t encoded_length = make_frame(payload, sizeof(payload), encoded);
	uart_frame_parser_t parser;
	uart_frame_view_t view;
	size_t pos;

	uart_frame_parser_init(&parser);
	for (pos = 0U; pos < ARRAY_SIZE(prefix); pos++)
	{
		CHECK(uart_frame_parser_process(&parser, prefix[pos], &view) ==
			  UART_FRAME_RESULT_IN_PROGRESS);
	}
	/* The prefix ends in A5, so start at the second magic byte. */
	for (pos = 1U; pos < encoded_length; pos++)
	{
		uart_frame_result_t expected = pos + 1U == encoded_length
										   ? UART_FRAME_RESULT_COMPLETE
										   : UART_FRAME_RESULT_IN_PROGRESS;
		CHECK(uart_frame_parser_process(&parser, encoded[pos], &view) == expected);
	}
	CHECK(view.payload_length == sizeof(payload));
	CHECK(view.payload[0] == payload[0]);
	return true;
}

static bool test_frame_parser_rejects_bad_lengths_and_crc_then_recovers(void)
{
	const uint8_t zero_length_header[] = {UART_FRAME_MAGIC_1,
		UART_FRAME_MAGIC_2, 0x00U, 0x00U};
	const uint8_t oversized_header[] = {UART_FRAME_MAGIC_1,
		UART_FRAME_MAGIC_2, UART_FRAME_MAX_PAYLOAD_SIZE + 1U, 0x00U};
	const uint8_t payload[] = {0x42U};
	uint8_t encoded[TEST_FRAME_MAX_SIZE];
	size_t encoded_length = make_frame(payload, sizeof(payload), encoded);
	uart_frame_parser_t parser;
	uart_frame_view_t view;
	size_t pos;

	uart_frame_parser_init(&parser);
	for (pos = 0U; pos < ARRAY_SIZE(zero_length_header); pos++)
	{
		uart_frame_result_t result = uart_frame_parser_process(&parser,
			zero_length_header[pos], &view);
		CHECK(result == (pos + 1U == ARRAY_SIZE(zero_length_header)
								? UART_FRAME_RESULT_ERROR
								: UART_FRAME_RESULT_IN_PROGRESS));
	}
	for (pos = 0U; pos < ARRAY_SIZE(oversized_header); pos++)
	{
		uart_frame_result_t result = uart_frame_parser_process(&parser,
			oversized_header[pos], &view);
		CHECK(result == (pos + 1U == ARRAY_SIZE(oversized_header)
								? UART_FRAME_RESULT_ERROR
								: UART_FRAME_RESULT_IN_PROGRESS));
	}

	encoded[encoded_length - 1U] ^= 0x01U;
	for (pos = 0U; pos < encoded_length; pos++)
	{
		uart_frame_result_t result = uart_frame_parser_process(&parser,
			encoded[pos], &view);
		CHECK(result == (pos + 1U == encoded_length
								? UART_FRAME_RESULT_ERROR
								: UART_FRAME_RESULT_IN_PROGRESS));
	}

	encoded_length = make_frame(payload, sizeof(payload), encoded);
	for (pos = 0U; pos < encoded_length; pos++)
	{
		uart_frame_result_t result = uart_frame_parser_process(&parser,
			encoded[pos], &view);
		CHECK(result == (pos + 1U == encoded_length
								? UART_FRAME_RESULT_COMPLETE
								: UART_FRAME_RESULT_IN_PROGRESS));
	}
	return true;
}

static bool test_frame_parser_rejects_null_arguments(void)
{
	uart_frame_parser_t parser;
	uart_frame_view_t view;

	uart_frame_parser_init(NULL);
	uart_frame_parser_init(&parser);
	CHECK(uart_frame_parser_process(NULL, 0U, &view) ==
		  UART_FRAME_RESULT_ERROR);
	CHECK(uart_frame_parser_process(&parser, 0U, NULL) ==
		  UART_FRAME_RESULT_ERROR);
	return true;
}

static bool test_decoder_decodes_each_command(void)
{
	device_DeviceCommand protobuf = device_DeviceCommand_init_zero;
	decoded_device_message_t decoded;
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
	size_t payload_length;

	protobuf.which_command = device_DeviceCommand_set_max_temp_tag;
	protobuf.command.set_max_temp.value = 32767;
	payload_length = encode_command(&protobuf, payload);
	CHECK(payload_length > 0U);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_OK);
	CHECK(decoded.type == DEVICE_MESSAGE_SET_MAX_TEMP);
	CHECK(decoded.value.temperature == 32767);

	protobuf = (device_DeviceCommand)device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_min_temp_tag;
	protobuf.command.set_min_temp.value = -32768;
	payload_length = encode_command(&protobuf, payload);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_OK);
	CHECK(decoded.type == DEVICE_MESSAGE_SET_MIN_TEMP);
	CHECK(decoded.value.temperature == -32768);

	protobuf = (device_DeviceCommand)device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_current_time_tag;
	protobuf.command.set_current_time.value_ms = UINT64_C(1721234567890);
	payload_length = encode_command(&protobuf, payload);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_OK);
	CHECK(decoded.type == DEVICE_MESSAGE_SET_CURRENT_TIME);
	CHECK(decoded.value.current_time_ms == UINT64_C(1721234567890));

	protobuf = (device_DeviceCommand)device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_temperature_unit_tag;
	protobuf.command.set_temperature_unit.unit =
		device_TemperatureUnit_TEMPERATURE_UNIT_FAHRENHEIT;
	payload_length = encode_command(&protobuf, payload);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_OK);
	CHECK(decoded.type == DEVICE_MESSAGE_SET_TEMPERATURE_UNIT);
	CHECK(decoded.value.temperature_unit ==
		  DEVICE_TEMPERATURE_UNIT_FAHRENHEIT);
	return true;
}

static bool test_decoder_decodes_ble_connection_state(void)
{
	device_DeviceMessage protobuf = device_DeviceMessage_init_zero;
	decoded_device_message_t decoded;
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
	size_t payload_length;

	protobuf.which_payload = device_DeviceMessage_bridge_status_tag;
	protobuf.payload.bridge_status.ble_connection_state =
		device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTING;
	payload_length = encode_message(&protobuf, payload);
	CHECK(payload_length > 0U);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_OK);
	CHECK(decoded.type == DEVICE_MESSAGE_BLE_CONNECTION_STATE);
	CHECK(decoded.value.ble_connection_state == BLE_CONNECTION_STATE_CONNECTING);

	protobuf.payload.bridge_status.ble_connection_state =
		device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTED;
	payload_length = encode_message(&protobuf, payload);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_OK);
	CHECK(decoded.type == DEVICE_MESSAGE_BLE_CONNECTION_STATE);
	CHECK(decoded.value.ble_connection_state == BLE_CONNECTION_STATE_CONNECTED);

	protobuf.payload.bridge_status.ble_connection_state =
		device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTING;
	payload_length = encode_message(&protobuf, payload);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_OK);
	CHECK(decoded.type == DEVICE_MESSAGE_BLE_CONNECTION_STATE);
	CHECK(decoded.value.ble_connection_state ==
		  BLE_CONNECTION_STATE_DISCONNECTING);

	protobuf.payload.bridge_status.ble_connection_state =
		device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTED;
	payload_length = encode_message(&protobuf, payload);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_OK);
	CHECK(decoded.type == DEVICE_MESSAGE_BLE_CONNECTION_STATE);
	CHECK(decoded.value.ble_connection_state ==
		  BLE_CONNECTION_STATE_DISCONNECTED);
	return true;
}

static bool test_decoder_rejects_out_of_range_temperatures(void)
{
	device_DeviceCommand protobuf = device_DeviceCommand_init_zero;
	decoded_device_message_t decoded;
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
	size_t payload_length;

	protobuf.which_command = device_DeviceCommand_set_max_temp_tag;
	protobuf.command.set_max_temp.value = 32768;
	payload_length = encode_command(&protobuf, payload);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_VALUE_OUT_OF_RANGE);

	protobuf = (device_DeviceCommand)device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_min_temp_tag;
	protobuf.command.set_min_temp.value = -32769;
	payload_length = encode_command(&protobuf, payload);
	CHECK(device_message_decode(payload, (uint16_t)payload_length, &decoded) ==
		  DEVICE_MESSAGE_DECODE_VALUE_OUT_OF_RANGE);
	return true;
}

static bool test_decoder_rejects_invalid_payloads(void)
{
	const uint8_t malformed[] = {0xFFU};
	const uint8_t unknown_field_only[] = {0x20U, 0x01U};
	decoded_device_message_t decoded;

	CHECK(device_message_decode(NULL, 1U, &decoded) ==
		  DEVICE_MESSAGE_DECODE_INVALID_ARGUMENT);
	CHECK(device_message_decode(malformed, 0U, &decoded) ==
		  DEVICE_MESSAGE_DECODE_INVALID_ARGUMENT);
	CHECK(device_message_decode(malformed, sizeof(malformed), NULL) ==
		  DEVICE_MESSAGE_DECODE_INVALID_ARGUMENT);
	CHECK(device_message_decode(malformed, sizeof(malformed), &decoded) ==
		  DEVICE_MESSAGE_DECODE_INVALID_PROTOBUF);
	CHECK(device_message_decode(unknown_field_only, sizeof(unknown_field_only),
			  &decoded) == DEVICE_MESSAGE_DECODE_MISSING_PAYLOAD);
	return true;
}

static bool test_receiver_initializes_and_rearms_uart(void)
{
	uart_command_receiver_t receiver;
	UART_HandleTypeDef huart = {0};

	memset(&receiver, 0xA5, sizeof(receiver));
	reset_uart_mock();
	uart_command_receiver_init(&receiver, &huart);
	CHECK(receiver.rx.huart == &huart);
	CHECK(receiver.rx.head == 0U && receiver.rx.tail == 0U);
	CHECK(receiver.stats.rx_byte_count == 0U);
	CHECK(!receiver.values.min_temp_updated);
	CHECK(!receiver.values.max_temp_updated);
	CHECK(!receiver.values.current_time_updated);
	CHECK(receiver.values.temperature_unit == DEVICE_TEMPERATURE_UNIT_CELSIUS);
	CHECK(!receiver.values.temperature_unit_updated);
	CHECK(receiver.values.ble_connection_state ==
		  BLE_CONNECTION_STATE_DISCONNECTED);
	CHECK(uart_command_receiver_rx_byte_ptr(&receiver) == &receiver.rx.byte);
	CHECK(uart_command_receiver_rx_byte_ptr(NULL) == NULL);

	receiver.rx.byte = 0x55U;
	uart_command_receiver_on_rx_complete(&receiver);
	CHECK(receiver.stats.rx_byte_count == 1U);
	CHECK(mock_receive_call_count == 1U);
	CHECK(mock_receive_huart == &huart);
	CHECK(mock_receive_data == &receiver.rx.byte);
	CHECK(mock_receive_size == 1U);
	return true;
}

static bool test_receiver_tracks_rearm_failures_and_overflow(void)
{
	uart_command_receiver_t receiver;
	UART_HandleTypeDef huart = {0};
	size_t pos;

	reset_uart_mock();
	mock_receive_result = HAL_ERROR;
	uart_command_receiver_init(&receiver, &huart);
	receiver.rx.byte = 0x11U;
	uart_command_receiver_on_rx_complete(&receiver);
	CHECK(receiver.stats.rx_rearm_error_count == 1U);

	reset_uart_mock();
	uart_command_receiver_init(&receiver, NULL);
	uart_command_receiver_on_rx_complete(&receiver);
	CHECK(receiver.stats.rx_rearm_error_count == 1U);
	CHECK(mock_receive_call_count == 0U);

	uart_command_receiver_init(&receiver, &huart);
	for (pos = 0U; pos < UART_COMMANDS_RX_LOG_SIZE; pos++)
	{
		receiver.rx.byte = 0x00U;
		uart_command_receiver_on_rx_complete(&receiver);
	}
	CHECK(receiver.stats.rx_byte_count == UART_COMMANDS_RX_LOG_SIZE);
	CHECK(receiver.stats.rx_overflow_count == 1U);
	uart_command_receiver_poll(&receiver);
	CHECK(receiver.rx.head == receiver.rx.tail);
	return true;
}

static bool test_receiver_applies_framed_commands_end_to_end(void)
{
	device_DeviceCommand protobuf = device_DeviceCommand_init_zero;
	uart_command_receiver_t receiver;
	UART_HandleTypeDef huart = {0};
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
	uint8_t frame[TEST_FRAME_MAX_SIZE];
	size_t payload_length;
	size_t frame_length;

	reset_uart_mock();
	uart_command_receiver_init(&receiver, &huart);

	protobuf.which_command = device_DeviceCommand_set_max_temp_tag;
	protobuf.command.set_max_temp.value = 255;
	payload_length = encode_command(&protobuf, payload);
	frame_length = make_frame(payload, payload_length, frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	CHECK(receiver.values.max_temp == 255);
	CHECK(receiver.values.max_temp_updated);

	protobuf = (device_DeviceCommand)device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_min_temp_tag;
	protobuf.command.set_min_temp.value = -55;
	payload_length = encode_command(&protobuf, payload);
	frame_length = make_frame(payload, payload_length, frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	CHECK(receiver.values.min_temp == -55);
	CHECK(receiver.values.min_temp_updated);

	protobuf = (device_DeviceCommand)device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_current_time_tag;
	protobuf.command.set_current_time.value_ms = UINT64_C(9876543210);
	payload_length = encode_command(&protobuf, payload);
	frame_length = make_frame(payload, payload_length, frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	CHECK(receiver.values.current_time_ms == UINT64_C(9876543210));
	CHECK(receiver.values.current_time_updated);

	protobuf = (device_DeviceCommand)device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_temperature_unit_tag;
	protobuf.command.set_temperature_unit.unit =
		device_TemperatureUnit_TEMPERATURE_UNIT_FAHRENHEIT;
	payload_length = encode_command(&protobuf, payload);
	frame_length = make_frame(payload, payload_length, frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	CHECK(receiver.values.temperature_unit ==
		  DEVICE_TEMPERATURE_UNIT_FAHRENHEIT);
	CHECK(receiver.values.temperature_unit_updated);

	{
		device_DeviceMessage message = device_DeviceMessage_init_zero;

		message.which_payload = device_DeviceMessage_bridge_status_tag;
		message.payload.bridge_status.ble_connection_state =
			device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTED;
		payload_length = encode_message(&message, payload);
		frame_length = make_frame(payload, payload_length, frame);
		receiver_feed(&receiver, frame, frame_length);
		uart_command_receiver_poll(&receiver);
		CHECK(receiver.values.ble_connection_state ==
			  BLE_CONNECTION_STATE_CONNECTED);
	}

	CHECK(receiver.stats.frame_error_count == 0U);
	CHECK(receiver.stats.protobuf_decode_error_count == 0U);
	CHECK(mock_receive_call_count == receiver.stats.rx_byte_count);
	return true;
}

static bool test_receiver_counts_frame_and_protobuf_errors(void)
{
	const uint8_t invalid_protobuf[] = {0xFFU};
	uart_command_receiver_t receiver;
	UART_HandleTypeDef huart = {0};
	uint8_t frame[TEST_FRAME_MAX_SIZE];
	size_t frame_length;

	reset_uart_mock();
	uart_command_receiver_init(&receiver, &huart);
	frame_length = make_frame(invalid_protobuf, sizeof(invalid_protobuf), frame);
	frame[frame_length - 1U] ^= 0x01U;
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	CHECK(receiver.stats.frame_error_count == 1U);
	CHECK(receiver.stats.protobuf_decode_error_count == 0U);

	frame_length = make_frame(invalid_protobuf, sizeof(invalid_protobuf), frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	CHECK(receiver.stats.frame_error_count == 1U);
	CHECK(receiver.stats.protobuf_decode_error_count == 1U);
	CHECK(receiver.stats.last_reported_frame_error_count == 1U);
	CHECK(receiver.stats.last_reported_protobuf_decode_error_count == 1U);
	return true;
}

typedef bool (*test_function_t)(void);

typedef struct
{
	const char *name;
	test_function_t function;
} test_case_t;

int main(void)
{
	const test_case_t tests[] = {
		{"frame parser accepts a valid frame", test_frame_parser_accepts_valid_frame},
		{"frame parser resynchronizes", test_frame_parser_resynchronizes_after_noise_and_repeated_magic},
		{"frame parser rejects corruption and recovers", test_frame_parser_rejects_bad_lengths_and_crc_then_recovers},
		{"frame parser validates arguments", test_frame_parser_rejects_null_arguments},
		{"decoder handles every command", test_decoder_decodes_each_command},
		{"decoder handles BLE connection state", test_decoder_decodes_ble_connection_state},
		{"decoder enforces temperature range", test_decoder_rejects_out_of_range_temperatures},
		{"decoder rejects invalid payloads", test_decoder_rejects_invalid_payloads},
		{"receiver initializes and rearms UART", test_receiver_initializes_and_rearms_uart},
		{"receiver tracks UART failures and overflow", test_receiver_tracks_rearm_failures_and_overflow},
		{"receiver applies framed commands", test_receiver_applies_framed_commands_end_to_end},
		{"receiver counts frame and protobuf errors", test_receiver_counts_frame_and_protobuf_errors},
	};
	size_t pos;
	size_t passed = 0U;

	for (pos = 0U; pos < ARRAY_SIZE(tests); pos++)
	{
		printf("[ RUN      ] %s\n", tests[pos].name);
		if (tests[pos].function())
		{
			printf("[       OK ] %s\n", tests[pos].name);
			passed++;
		}
		else
		{
			printf("[  FAILED  ] %s\n", tests[pos].name);
		}
	}

	printf("\n%zu/%zu tests passed\n", passed, ARRAY_SIZE(tests));
	return passed == ARRAY_SIZE(tests) ? 0 : 1;
}
