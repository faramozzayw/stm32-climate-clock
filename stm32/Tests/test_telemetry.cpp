#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <limits>

extern "C"
{
#include "unity.h"
}

#include "command_receiver/uart_frame.h"
#include "device.pb.h"
#include "pb_decode.h"
#include "telemetry.hpp"

namespace telemetry = climate_clock::telemetry;

static bool decode_frame(
	const uint8_t *frame,
	uint16_t frame_length,
	device_DeviceMessage *message)
{
	uart_frame_parser_t parser;
	uart_frame_view_t view{};
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

static uint16_t payload_length(const uint8_t *frame)
{
	return static_cast<uint16_t>(
		static_cast<uint16_t>(frame[2]) |
		(static_cast<uint16_t>(frame[3]) << 8U));
}

static void test_telemetry_round_trips_measurement(void)
{
	uint8_t frame[telemetry::max_frame_size];
	uint16_t frame_length;
	device_DeviceMessage message = device_DeviceMessage_init_zero;

	TEST_ASSERT_TRUE(telemetry::encode_measurement(
		-55, 60075U, frame, frame_length));
	TEST_ASSERT_TRUE(frame_length <= sizeof(frame));
	TEST_ASSERT_TRUE(payload_length(frame) <= 20U);
	TEST_ASSERT_TRUE(decode_frame(frame, frame_length, &message));
	TEST_ASSERT_TRUE(message.which_payload == device_DeviceMessage_telemetry_tag);
	TEST_ASSERT_TRUE(
		message.payload.telemetry.which_data ==
		device_DeviceTelemetry_measurement_tag);
	TEST_ASSERT_TRUE(
		message.payload.telemetry.data.measurement
			.temperature_tenths_celsius == -55);
	TEST_ASSERT_TRUE(
		message.payload.telemetry.data.measurement
			.has_humidity_milli_percent);
	TEST_ASSERT_EQUAL_UINT32(
		60075U,
		message.payload.telemetry.data.measurement
			.humidity_milli_percent);
}

static void test_telemetry_omits_unavailable_humidity(void)
{
	uint8_t frame[telemetry::max_frame_size];
	uint16_t frame_length;
	device_DeviceMessage message = device_DeviceMessage_init_zero;

	TEST_ASSERT_TRUE(telemetry::encode_measurement(
		0, std::nullopt, frame, frame_length));
	TEST_ASSERT_TRUE(decode_frame(frame, frame_length, &message));
	TEST_ASSERT_FALSE(
		message.payload.telemetry.data.measurement
			.has_humidity_milli_percent);
}

static void test_telemetry_round_trips_settings(void)
{
	uint8_t frame[telemetry::max_frame_size];
	uint16_t frame_length;
	device_DeviceMessage message = device_DeviceMessage_init_zero;

	TEST_ASSERT_TRUE(telemetry::encode_settings(
		100,
		300,
		300U,
		650U,
		TEMPERATURE_UNIT_FAHRENHEIT,
		frame,
		frame_length));
	TEST_ASSERT_TRUE(payload_length(frame) <= 20U);
	TEST_ASSERT_TRUE(decode_frame(frame, frame_length, &message));
	TEST_ASSERT_TRUE(
		message.payload.telemetry.which_data ==
		device_DeviceTelemetry_settings_tag);

	const auto &settings = message.payload.telemetry.data.settings;
	TEST_ASSERT_TRUE(settings.min_temperature_tenths_celsius == 100);
	TEST_ASSERT_TRUE(settings.max_temperature_tenths_celsius == 300);
	TEST_ASSERT_EQUAL_UINT32(
		300U,
		settings.min_humidity_tenths_percent);
	TEST_ASSERT_EQUAL_UINT32(
		650U,
		settings.max_humidity_tenths_percent);
	TEST_ASSERT_EQUAL_INT(
		device_TemperatureUnit_TEMPERATURE_UNIT_FAHRENHEIT,
		settings.temperature_unit);
}

static void test_telemetry_rejects_small_frame_buffer(void)
{
	uint8_t frame[1];
	uint16_t frame_length;

	TEST_ASSERT_FALSE(telemetry::encode_measurement(
		0, 60075U, frame, frame_length));
}

static void test_telemetry_variants_fit_default_ble_notification(void)
{
	uint8_t frame[telemetry::max_frame_size];
	uint16_t frame_length;

	TEST_ASSERT_TRUE(telemetry::encode_measurement(
		std::numeric_limits<int16_t>::min(),
		std::numeric_limits<uint32_t>::max(),
		frame,
		frame_length));
	TEST_ASSERT_TRUE(payload_length(frame) <= 20U);

	TEST_ASSERT_TRUE(telemetry::encode_settings(
		std::numeric_limits<int16_t>::min(),
		std::numeric_limits<int16_t>::max(),
		1000U,
		1000U,
		TEMPERATURE_UNIT_FAHRENHEIT,
		frame,
		frame_length));
	TEST_ASSERT_TRUE(payload_length(frame) <= 20U);
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
	RUN_TEST(test_telemetry_round_trips_measurement);
	RUN_TEST(test_telemetry_omits_unavailable_humidity);
	RUN_TEST(test_telemetry_round_trips_settings);
	RUN_TEST(test_telemetry_rejects_small_frame_buffer);
	RUN_TEST(test_telemetry_variants_fit_default_ble_notification);
	return UNITY_END();
}
