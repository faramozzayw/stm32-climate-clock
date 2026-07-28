#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <variant>

extern "C"
{
#include "unity.h"
}

#include "command_receiver/device_message_decoder.hpp"
#include "command_receiver/uart_command_receiver.h"
#include "command_receiver/uart_frame.h"
#include "device.pb.h"
#include "pb_encode.h"

using climate_clock::BleConnectionStateChanged;
using climate_clock::decode_device_message;
using climate_clock::DecodeError;
using climate_clock::DecodeResult;
using climate_clock::SetCurrentTime;
using climate_clock::SetMaximumTemperature;
using climate_clock::SetMinimumTemperature;
using climate_clock::SetTemperatureUnit;

constexpr std::size_t test_frame_max_size =
	2U + 2U + UART_FRAME_MAX_PAYLOAD_SIZE + 2U;

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
	std::memcpy(&frame[4], payload, payload_length);
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

template <typename Message>
static const Message *decoded_value(const DecodeResult &result)
{
	if (!result)
	{
		return nullptr;
	}

	return std::get_if<Message>(&result.value());
}

static void receiver_feed(uart_command_receiver_t *receiver,
	const uint8_t *bytes, size_t length)
{
	size_t pos;

	for (pos = 0U; pos < length; pos++)
	{
		uart_command_receiver_push_byte(receiver, bytes[pos]);
	}
}

static void test_frame_parser_accepts_valid_frame(void)
{
	const uint8_t payload[] = {0x08U, 0x96U, 0x01U};
	uint8_t encoded[test_frame_max_size];
	size_t encoded_length = make_frame(payload, sizeof(payload), encoded);
	uart_frame_parser_t parser;
	uart_frame_view_t view{};
	size_t pos;

	uart_frame_parser_init(&parser);
	for (pos = 0U; pos + 1U < encoded_length; pos++)
	{
		TEST_ASSERT_TRUE(uart_frame_parser_process(&parser, encoded[pos], &view) ==
						 UART_FRAME_RESULT_IN_PROGRESS);
	}
	TEST_ASSERT_TRUE(uart_frame_parser_process(&parser, encoded[pos], &view) ==
					 UART_FRAME_RESULT_COMPLETE);
	TEST_ASSERT_TRUE(view.payload_length == sizeof(payload));
	TEST_ASSERT_TRUE(
		std::memcmp(view.payload, payload, sizeof(payload)) == 0);
	TEST_ASSERT_TRUE(parser.state == UART_FRAME_WAIT_MAGIC_1);
}

static void test_frame_parser_resynchronizes_after_noise_and_repeated_magic(void)
{
	const uint8_t payload[] = {0x01U};
	const uint8_t prefix[] = {0x00U, UART_FRAME_MAGIC_1, 0x00U,
		UART_FRAME_MAGIC_1};
	uint8_t encoded[test_frame_max_size];
	size_t encoded_length = make_frame(payload, sizeof(payload), encoded);
	uart_frame_parser_t parser;
	uart_frame_view_t view;
	size_t pos;

	uart_frame_parser_init(&parser);
	for (pos = 0U; pos < std::size(prefix); pos++)
	{
		TEST_ASSERT_TRUE(uart_frame_parser_process(&parser, prefix[pos], &view) ==
						 UART_FRAME_RESULT_IN_PROGRESS);
	}
	/* The prefix ends in A5, so start at the second magic byte. */
	for (pos = 1U; pos < encoded_length; pos++)
	{
		uart_frame_result_t expected = pos + 1U == encoded_length
										   ? UART_FRAME_RESULT_COMPLETE
										   : UART_FRAME_RESULT_IN_PROGRESS;
		TEST_ASSERT_TRUE(uart_frame_parser_process(&parser, encoded[pos], &view) == expected);
	}
	TEST_ASSERT_TRUE(view.payload_length == sizeof(payload));
	TEST_ASSERT_TRUE(view.payload[0] == payload[0]);
}

static void test_frame_parser_rejects_bad_lengths_and_crc_then_recovers(void)
{
	const uint8_t zero_length_header[] = {UART_FRAME_MAGIC_1,
		UART_FRAME_MAGIC_2, 0x00U, 0x00U};
	const uint8_t oversized_header[] = {UART_FRAME_MAGIC_1,
		UART_FRAME_MAGIC_2, UART_FRAME_MAX_PAYLOAD_SIZE + 1U, 0x00U};
	const uint8_t payload[] = {0x42U};
	uint8_t encoded[test_frame_max_size];
	size_t encoded_length = make_frame(payload, sizeof(payload), encoded);
	uart_frame_parser_t parser;
	uart_frame_view_t view;
	size_t pos;

	uart_frame_parser_init(&parser);
	for (pos = 0U; pos < std::size(zero_length_header); pos++)
	{
		uart_frame_result_t result = uart_frame_parser_process(&parser,
			zero_length_header[pos], &view);
		TEST_ASSERT_TRUE(result == (pos + 1U == std::size(zero_length_header)
										   ? UART_FRAME_RESULT_ERROR
										   : UART_FRAME_RESULT_IN_PROGRESS));
	}
	for (pos = 0U; pos < std::size(oversized_header); pos++)
	{
		uart_frame_result_t result = uart_frame_parser_process(&parser,
			oversized_header[pos], &view);
		TEST_ASSERT_TRUE(result == (pos + 1U == std::size(oversized_header)
										   ? UART_FRAME_RESULT_ERROR
										   : UART_FRAME_RESULT_IN_PROGRESS));
	}

	encoded[encoded_length - 1U] ^= 0x01U;
	for (pos = 0U; pos < encoded_length; pos++)
	{
		uart_frame_result_t result = uart_frame_parser_process(&parser,
			encoded[pos], &view);
		TEST_ASSERT_TRUE(result == (pos + 1U == encoded_length
										   ? UART_FRAME_RESULT_ERROR
										   : UART_FRAME_RESULT_IN_PROGRESS));
	}

	encoded_length = make_frame(payload, sizeof(payload), encoded);
	for (pos = 0U; pos < encoded_length; pos++)
	{
		uart_frame_result_t result = uart_frame_parser_process(&parser,
			encoded[pos], &view);
		TEST_ASSERT_TRUE(result == (pos + 1U == encoded_length
										   ? UART_FRAME_RESULT_COMPLETE
										   : UART_FRAME_RESULT_IN_PROGRESS));
	}
}

static void test_frame_parser_rejects_null_arguments(void)
{
	uart_frame_parser_t parser;
	uart_frame_view_t view;

	uart_frame_parser_init(&parser);
	TEST_ASSERT_TRUE(uart_frame_parser_process(nullptr, 0U, &view) ==
					 UART_FRAME_RESULT_ERROR);
	TEST_ASSERT_TRUE(uart_frame_parser_process(&parser, 0U, nullptr) ==
					 UART_FRAME_RESULT_ERROR);
}

static void test_decoder_decodes_each_command(void)
{
	device_DeviceCommand protobuf = device_DeviceCommand_init_zero;
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
	size_t payload_length;

	protobuf.which_command = device_DeviceCommand_set_max_temp_tag;
	protobuf.command.set_max_temp.value = 32767;
	payload_length = encode_command(&protobuf, payload);
	TEST_ASSERT_TRUE(payload_length > 0U);
	auto result = decode_device_message(payload, payload_length);
	const auto *maximum = decoded_value<SetMaximumTemperature>(result);
	TEST_ASSERT_NOT_NULL(maximum);
	TEST_ASSERT_TRUE(maximum->temperature == 32767);

	protobuf = device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_min_temp_tag;
	protobuf.command.set_min_temp.value = -32768;
	payload_length = encode_command(&protobuf, payload);
	result = decode_device_message(payload, payload_length);
	const auto *minimum = decoded_value<SetMinimumTemperature>(result);
	TEST_ASSERT_NOT_NULL(minimum);
	TEST_ASSERT_TRUE(minimum->temperature == -32768);

	protobuf = device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_current_time_tag;
	protobuf.command.set_current_time.value_ms = UINT64_C(1721234567890);
	payload_length = encode_command(&protobuf, payload);
	result = decode_device_message(payload, payload_length);
	const auto *current_time = decoded_value<SetCurrentTime>(result);
	TEST_ASSERT_NOT_NULL(current_time);
	TEST_ASSERT_TRUE(
		current_time->time_ms == UINT64_C(1721234567890));

	protobuf = device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_temperature_unit_tag;
	protobuf.command.set_temperature_unit.unit =
		device_TemperatureUnit_TEMPERATURE_UNIT_FAHRENHEIT;
	payload_length = encode_command(&protobuf, payload);
	result = decode_device_message(payload, payload_length);
	const auto *unit = decoded_value<SetTemperatureUnit>(result);
	TEST_ASSERT_NOT_NULL(unit);
	TEST_ASSERT_TRUE(
		unit->unit == DEVICE_TEMPERATURE_UNIT_FAHRENHEIT);
}

static void test_decoder_decodes_ble_connection_state(void)
{
	device_DeviceMessage protobuf = device_DeviceMessage_init_zero;
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
	size_t payload_length;

	protobuf.which_payload = device_DeviceMessage_bridge_status_tag;
	protobuf.payload.bridge_status.ble_connection_state =
		device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTING;
	payload_length = encode_message(&protobuf, payload);
	TEST_ASSERT_TRUE(payload_length > 0U);
	auto result = decode_device_message(payload, payload_length);
	auto *connection =
		decoded_value<BleConnectionStateChanged>(result);
	TEST_ASSERT_NOT_NULL(connection);
	TEST_ASSERT_TRUE(connection->state == BLE_CONNECTION_STATE_CONNECTING);

	protobuf.payload.bridge_status.ble_connection_state =
		device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTED;
	payload_length = encode_message(&protobuf, payload);
	result = decode_device_message(payload, payload_length);
	connection = decoded_value<BleConnectionStateChanged>(result);
	TEST_ASSERT_NOT_NULL(connection);
	TEST_ASSERT_TRUE(connection->state == BLE_CONNECTION_STATE_CONNECTED);

	protobuf.payload.bridge_status.ble_connection_state =
		device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTING;
	payload_length = encode_message(&protobuf, payload);
	result = decode_device_message(payload, payload_length);
	connection = decoded_value<BleConnectionStateChanged>(result);
	TEST_ASSERT_NOT_NULL(connection);
	TEST_ASSERT_TRUE(
		connection->state == BLE_CONNECTION_STATE_DISCONNECTING);

	protobuf.payload.bridge_status.ble_connection_state =
		device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTED;
	payload_length = encode_message(&protobuf, payload);
	result = decode_device_message(payload, payload_length);
	connection = decoded_value<BleConnectionStateChanged>(result);
	TEST_ASSERT_NOT_NULL(connection);
	TEST_ASSERT_TRUE(
		connection->state == BLE_CONNECTION_STATE_DISCONNECTED);
}

static void test_decoder_rejects_out_of_range_temperatures(void)
{
	device_DeviceCommand protobuf = device_DeviceCommand_init_zero;
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
	size_t payload_length;

	protobuf.which_command = device_DeviceCommand_set_max_temp_tag;
	protobuf.command.set_max_temp.value = 32768;
	payload_length = encode_command(&protobuf, payload);
	auto result = decode_device_message(payload, payload_length);
	TEST_ASSERT_FALSE(result);
	TEST_ASSERT_TRUE(result.error() == DecodeError::value_out_of_range);

	protobuf = device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_min_temp_tag;
	protobuf.command.set_min_temp.value = -32769;
	payload_length = encode_command(&protobuf, payload);
	result = decode_device_message(payload, payload_length);
	TEST_ASSERT_FALSE(result);
	TEST_ASSERT_TRUE(result.error() == DecodeError::value_out_of_range);
}

static void test_decoder_rejects_invalid_payloads(void)
{
	const uint8_t malformed[] = {0xFFU};
	const uint8_t unknown_field_only[] = {0x20U, 0x01U};

	auto result = decode_device_message(nullptr, 1U);
	TEST_ASSERT_FALSE(result);
	TEST_ASSERT_TRUE(result.error() == DecodeError::invalid_argument);

	result = decode_device_message(malformed, 0U);
	TEST_ASSERT_FALSE(result);
	TEST_ASSERT_TRUE(result.error() == DecodeError::invalid_argument);

	result = decode_device_message(malformed, sizeof(malformed));
	TEST_ASSERT_FALSE(result);
	TEST_ASSERT_TRUE(result.error() == DecodeError::invalid_protobuf);

	result = decode_device_message(
		unknown_field_only,
		sizeof(unknown_field_only));
	TEST_ASSERT_FALSE(result);
	TEST_ASSERT_TRUE(result.error() == DecodeError::missing_payload);
}

static void test_receiver_initializes_and_accepts_bytes(void)
{
	uart_command_receiver_t receiver;

	std::memset(&receiver, 0xA5, sizeof(receiver));
	uart_command_receiver_init(&receiver);
	TEST_ASSERT_TRUE(receiver.rx.head == receiver.rx.tail);
	TEST_ASSERT_TRUE(receiver.stats.rx_byte_count == 0U);
	TEST_ASSERT_TRUE(!receiver.values.min_temp_updated);
	TEST_ASSERT_TRUE(!receiver.values.max_temp_updated);
	TEST_ASSERT_TRUE(!receiver.values.current_time_updated);
	TEST_ASSERT_TRUE(receiver.values.temperature_unit == DEVICE_TEMPERATURE_UNIT_CELSIUS);
	TEST_ASSERT_TRUE(!receiver.values.temperature_unit_updated);
	TEST_ASSERT_TRUE(receiver.values.ble_connection_state ==
					 BLE_CONNECTION_STATE_DISCONNECTED);
	uart_command_receiver_push_byte(&receiver, 0x55U);
	TEST_ASSERT_TRUE(receiver.stats.rx_byte_count == 1U);
}

static void test_receiver_tracks_overflow(void)
{
	uart_command_receiver_t receiver;
	size_t pos;

	uart_command_receiver_init(&receiver);
	for (pos = 0U; pos < UART_COMMAND_RECEIVER_RX_CAPACITY; pos++)
	{
		uart_command_receiver_push_byte(&receiver, 0x00U);
	}
	TEST_ASSERT_TRUE(receiver.stats.rx_byte_count == UART_COMMAND_RECEIVER_RX_CAPACITY);
	TEST_ASSERT_TRUE(receiver.stats.rx_overflow_count == 1U);
	uart_command_receiver_poll(&receiver);
	TEST_ASSERT_TRUE(receiver.rx.head == receiver.rx.tail);
}

static void test_receiver_applies_framed_commands_end_to_end(void)
{
	device_DeviceCommand protobuf = device_DeviceCommand_init_zero;
	uart_command_receiver_t receiver;
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
	uint8_t frame[test_frame_max_size];
	size_t payload_length;
	size_t frame_length;

	uart_command_receiver_init(&receiver);

	protobuf.which_command = device_DeviceCommand_set_max_temp_tag;
	protobuf.command.set_max_temp.value = 255;
	payload_length = encode_command(&protobuf, payload);
	frame_length = make_frame(payload, payload_length, frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	TEST_ASSERT_TRUE(receiver.values.max_temp == 255);
	TEST_ASSERT_TRUE(receiver.values.max_temp_updated);

	protobuf = device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_min_temp_tag;
	protobuf.command.set_min_temp.value = -55;
	payload_length = encode_command(&protobuf, payload);
	frame_length = make_frame(payload, payload_length, frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	TEST_ASSERT_TRUE(receiver.values.min_temp == -55);
	TEST_ASSERT_TRUE(receiver.values.min_temp_updated);

	protobuf = device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_current_time_tag;
	protobuf.command.set_current_time.value_ms = UINT64_C(9876543210);
	payload_length = encode_command(&protobuf, payload);
	frame_length = make_frame(payload, payload_length, frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	TEST_ASSERT_TRUE(receiver.values.current_time_ms == UINT64_C(9876543210));
	TEST_ASSERT_TRUE(receiver.values.current_time_updated);

	protobuf = device_DeviceCommand_init_zero;
	protobuf.which_command = device_DeviceCommand_set_temperature_unit_tag;
	protobuf.command.set_temperature_unit.unit =
		device_TemperatureUnit_TEMPERATURE_UNIT_FAHRENHEIT;
	payload_length = encode_command(&protobuf, payload);
	frame_length = make_frame(payload, payload_length, frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	TEST_ASSERT_TRUE(receiver.values.temperature_unit ==
					 DEVICE_TEMPERATURE_UNIT_FAHRENHEIT);
	TEST_ASSERT_TRUE(receiver.values.temperature_unit_updated);

	{
		device_DeviceMessage message = device_DeviceMessage_init_zero;

		message.which_payload = device_DeviceMessage_bridge_status_tag;
		message.payload.bridge_status.ble_connection_state =
			device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTED;
		payload_length = encode_message(&message, payload);
		frame_length = make_frame(payload, payload_length, frame);
		receiver_feed(&receiver, frame, frame_length);
		uart_command_receiver_poll(&receiver);
		TEST_ASSERT_TRUE(receiver.values.ble_connection_state ==
						 BLE_CONNECTION_STATE_CONNECTED);
	}

	TEST_ASSERT_TRUE(receiver.stats.frame_error_count == 0U);
	TEST_ASSERT_TRUE(receiver.stats.protobuf_decode_error_count == 0U);
}

static void test_receiver_counts_frame_and_protobuf_errors(void)
{
	const uint8_t invalid_protobuf[] = {0xFFU};
	uart_command_receiver_t receiver;
	uint8_t frame[test_frame_max_size];
	size_t frame_length;

	uart_command_receiver_init(&receiver);
	frame_length = make_frame(invalid_protobuf, sizeof(invalid_protobuf), frame);
	frame[frame_length - 1U] ^= 0x01U;
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	TEST_ASSERT_TRUE(receiver.stats.frame_error_count == 1U);
	TEST_ASSERT_TRUE(receiver.stats.protobuf_decode_error_count == 0U);

	frame_length = make_frame(invalid_protobuf, sizeof(invalid_protobuf), frame);
	receiver_feed(&receiver, frame, frame_length);
	uart_command_receiver_poll(&receiver);
	TEST_ASSERT_TRUE(receiver.stats.frame_error_count == 1U);
	TEST_ASSERT_TRUE(receiver.stats.protobuf_decode_error_count == 1U);
	TEST_ASSERT_TRUE(receiver.stats.last_reported_frame_error_count == 1U);
	TEST_ASSERT_TRUE(receiver.stats.last_reported_protobuf_decode_error_count == 1U);
}

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_frame_parser_accepts_valid_frame);
	RUN_TEST(test_frame_parser_resynchronizes_after_noise_and_repeated_magic);
	RUN_TEST(test_frame_parser_rejects_bad_lengths_and_crc_then_recovers);
	RUN_TEST(test_frame_parser_rejects_null_arguments);
	RUN_TEST(test_decoder_decodes_each_command);
	RUN_TEST(test_decoder_decodes_ble_connection_state);
	RUN_TEST(test_decoder_rejects_out_of_range_temperatures);
	RUN_TEST(test_decoder_rejects_invalid_payloads);
	RUN_TEST(test_receiver_initializes_and_accepts_bytes);
	RUN_TEST(test_receiver_tracks_overflow);
	RUN_TEST(test_receiver_applies_framed_commands_end_to_end);
	RUN_TEST(test_receiver_counts_frame_and_protobuf_errors);
	return UNITY_END();
}
