#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "unity.h"
#include "command_receiver/uart_frame.h"
#include "device.pb.h"
#include "pb_decode.h"
#include "telemetry.h"

static bool decode_frame(
	const uint8_t *frame,
	uint16_t frame_length,
	device_DeviceMessage *message)
{
	uart_frame_parser_t parser;
	uart_frame_view_t view = {0};
	uint16_t pos;

	uart_frame_parser_init(&parser);
	for (pos = 0U; pos < frame_length; pos++)
	{
		uart_frame_result_t result =
			uart_frame_parser_process(&parser, frame[pos], &view);

		if (pos + 1U < frame_length)
		{
			if (result != UART_FRAME_RESULT_IN_PROGRESS)
			{
				return false;
			}
		}
		else if (result != UART_FRAME_RESULT_COMPLETE)
		{
			return false;
		}
	}

	{
		pb_istream_t stream =
			pb_istream_from_buffer(view.payload, view.payload_length);
		return pb_decode(&stream, device_DeviceMessage_fields, message);
	}
}

static void test_telemetry_round_trips_temperature_values(void)
{
	uint8_t frame[TELEMETRY_FRAME_MAX_SIZE];
	uint16_t frame_length;
	device_DeviceMessage message = device_DeviceMessage_init_zero;

	TEST_ASSERT_TRUE(telemetry_encode_temperature(
		-55, 100, 300, frame, sizeof(frame), &frame_length));
	TEST_ASSERT_TRUE(frame_length <= sizeof(frame));
	TEST_ASSERT_TRUE(decode_frame(frame, frame_length, &message));
	TEST_ASSERT_TRUE(message.which_payload == device_DeviceMessage_telemetry_tag);
	TEST_ASSERT_TRUE(message.payload.telemetry.current_temp == -55);
	TEST_ASSERT_TRUE(message.payload.telemetry.min_temp == 100);
	TEST_ASSERT_TRUE(message.payload.telemetry.max_temp == 300);
}

static void test_telemetry_rejects_invalid_destinations(void)
{
	uint8_t frame[TELEMETRY_FRAME_MAX_SIZE];
	uint16_t frame_length;

	TEST_ASSERT_TRUE(!telemetry_encode_temperature(
		0, 100, 300, NULL, sizeof(frame), &frame_length));
	TEST_ASSERT_TRUE(!telemetry_encode_temperature(
		0, 100, 300, frame, sizeof(frame), NULL));
	TEST_ASSERT_TRUE(!telemetry_encode_temperature(
		0, 100, 300, frame, 1U, &frame_length));
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
	RUN_TEST(test_telemetry_round_trips_temperature_values);
	RUN_TEST(test_telemetry_rejects_invalid_destinations);
	return UNITY_END();
}
