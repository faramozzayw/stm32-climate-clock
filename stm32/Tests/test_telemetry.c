#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "command_receiver/uart_frame.h"
#include "device.pb.h"
#include "pb_decode.h"
#include "telemetry.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#define CHECK(condition)                                                       \
	do                                                                         \
	{                                                                          \
		if (!(condition))                                                      \
		{                                                                      \
			printf("    CHECK failed at line %d: %s\n", __LINE__, #condition); \
			return false;                                                      \
		}                                                                      \
	} while (0)

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

static bool test_telemetry_round_trips_temperature_values(void)
{
	uint8_t frame[TELEMETRY_FRAME_MAX_SIZE];
	uint16_t frame_length;
	device_DeviceMessage message = device_DeviceMessage_init_zero;

	CHECK(telemetry_encode_temperature(
		-55, 100, 300, frame, sizeof(frame), &frame_length));
	CHECK(frame_length <= sizeof(frame));
	CHECK(decode_frame(frame, frame_length, &message));
	CHECK(message.which_payload == device_DeviceMessage_telemetry_tag);
	CHECK(message.payload.telemetry.current_temp == -55);
	CHECK(message.payload.telemetry.min_temp == 100);
	CHECK(message.payload.telemetry.max_temp == 300);
	return true;
}

static bool test_telemetry_rejects_invalid_destinations(void)
{
	uint8_t frame[TELEMETRY_FRAME_MAX_SIZE];
	uint16_t frame_length;

	CHECK(!telemetry_encode_temperature(
		0, 100, 300, NULL, sizeof(frame), &frame_length));
	CHECK(!telemetry_encode_temperature(
		0, 100, 300, frame, sizeof(frame), NULL));
	CHECK(!telemetry_encode_temperature(
		0, 100, 300, frame, 1U, &frame_length));
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
		{"telemetry round-trips temperature values", test_telemetry_round_trips_temperature_values},
		{"telemetry rejects invalid destinations", test_telemetry_rejects_invalid_destinations},
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
