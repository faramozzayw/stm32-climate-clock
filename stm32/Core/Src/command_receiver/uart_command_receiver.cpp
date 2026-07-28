#include "command_receiver/uart_command_receiver.h"

#include <cstdio>
#include <variant>

#include "command_receiver/device_message_decoder.hpp"
#include "utils/byte_ring_buffer.hpp"

using climate_clock::BleConnectionStateChanged;
using climate_clock::ByteRingBuffer;
using climate_clock::decode_device_message;
using climate_clock::DecodedDeviceMessage;
using climate_clock::SetCurrentTime;
using climate_clock::SetMaximumTemperature;
using climate_clock::SetMinimumTemperature;
using climate_clock::SetTemperatureUnit;

static ByteRingBuffer rx_buffer(uart_command_receiver_t &commands)
{
	return ByteRingBuffer{
		commands.rx.storage,
		commands.rx.head,
		commands.rx.tail};
}

static void print_temperature(const char *command, int16_t temperature)
{
	int32_t signed_value = temperature;
	uint32_t magnitude = (uint32_t)(signed_value < 0 ? -signed_value : signed_value);

	std::printf("[USART1] %s = %s%lu.%lu C\r\n",
		command,
		signed_value < 0 ? "-" : "",
		(unsigned long)(magnitude / 10U),
		(unsigned long)(magnitude % 10U));
}

static void apply_message(
	uart_command_receiver_t &commands,
	const SetMaximumTemperature &command)
{
	commands.values.max_temp = command.temperature;
	commands.values.max_temp_updated = true;
	print_temperature("SetMaxTemp", commands.values.max_temp);
}

static void apply_message(
	uart_command_receiver_t &commands,
	const SetMinimumTemperature &command)
{
	commands.values.min_temp = command.temperature;
	commands.values.min_temp_updated = true;
	print_temperature("SetMinTemp", commands.values.min_temp);
}

static void apply_message(
	uart_command_receiver_t &commands,
	const SetCurrentTime &command)
{
	commands.values.current_time_ms = command.time_ms;
	commands.values.current_time_updated = true;
	std::printf("[USART1] SetCurrentTime received\r\n");
}

static void apply_message(
	uart_command_receiver_t &commands,
	const SetTemperatureUnit &command)
{
	commands.values.temperature_unit = command.unit;
	commands.values.temperature_unit_updated = true;
	std::printf(
		"[USART1] Temperature unit = %c\r\n",
		commands.values.temperature_unit ==
				DEVICE_TEMPERATURE_UNIT_FAHRENHEIT
			? 'F'
			: 'C');
}

static void apply_message(
	uart_command_receiver_t &commands,
	const BleConnectionStateChanged &message)
{
	commands.values.ble_connection_state = message.state;

	switch (message.state)
	{
	case BLE_CONNECTION_STATE_DISCONNECTED:
		std::printf("[USART1] BLE disconnected\r\n");
		break;

	case BLE_CONNECTION_STATE_CONNECTING:
		std::printf("[USART1] BLE connecting\r\n");
		break;

	case BLE_CONNECTION_STATE_CONNECTED:
		std::printf("[USART1] BLE connected\r\n");
		break;

	case BLE_CONNECTION_STATE_DISCONNECTING:
		std::printf("[USART1] BLE disconnecting\r\n");
		break;
	}
}

static void apply_decoded_message(
	uart_command_receiver_t &commands,
	const DecodedDeviceMessage &message)
{
	std::visit(
		[&commands](const auto &decoded) {
			apply_message(commands, decoded);
		},
		message);
}

static void process_rx_byte(
	uart_command_receiver_t &commands,
	uint8_t byte)
{
	uart_frame_view_t frame;
	uart_frame_result_t frame_result = uart_frame_parser_process(
		&commands.frame_parser, byte, &frame);

	if (frame_result == UART_FRAME_RESULT_ERROR)
	{
		commands.stats.frame_error_count++;
	}
	else if (frame_result == UART_FRAME_RESULT_COMPLETE)
	{
		const auto decoded = decode_device_message(
			frame.payload,
			frame.payload_length);

		if (!decoded)
		{
			commands.stats.protobuf_decode_error_count++;
		}
		else
		{
			apply_decoded_message(commands, decoded.value());
		}
	}
}

void uart_command_receiver_init(uart_command_receiver_t *commands)
{
	rx_buffer(*commands).reset();
	uart_frame_parser_init(&commands->frame_parser);

	commands->values.min_temp_updated = false;
	commands->values.max_temp_updated = false;
	commands->values.current_time_updated = false;
	commands->values.temperature_unit = DEVICE_TEMPERATURE_UNIT_CELSIUS;
	commands->values.temperature_unit_updated = false;
	commands->values.ble_connection_state = BLE_CONNECTION_STATE_DISCONNECTED;

	commands->stats.rx_byte_count = 0U;
	commands->stats.rx_overflow_count = 0U;
	commands->stats.frame_error_count = 0U;
	commands->stats.protobuf_decode_error_count = 0U;
	commands->stats.last_reported_rx_overflow_count = 0U;
	commands->stats.last_reported_frame_error_count = 0U;
	commands->stats.last_reported_protobuf_decode_error_count = 0U;
}

void uart_command_receiver_push_byte(
	uart_command_receiver_t *commands,
	uint8_t byte)
{
	commands->stats.rx_byte_count++;
	if (!rx_buffer(*commands).push(byte))
	{
		commands->stats.rx_overflow_count++;
	}
}

void uart_command_receiver_poll(uart_command_receiver_t *commands)
{
	while (true)
	{
		uint8_t byte;

		if (!rx_buffer(*commands).pop(byte))
		{
			break;
		}

		process_rx_byte(*commands, byte);
	}

	if (commands->stats.last_reported_rx_overflow_count !=
		commands->stats.rx_overflow_count)
	{
		commands->stats.last_reported_rx_overflow_count =
			commands->stats.rx_overflow_count;
		std::printf("[USART1] RX buffer overflows: %lu\r\n",
			(unsigned long)commands->stats.rx_overflow_count);
	}

	if (commands->stats.last_reported_frame_error_count !=
		commands->stats.frame_error_count)
	{
		commands->stats.last_reported_frame_error_count =
			commands->stats.frame_error_count;
		std::printf("[USART1] UART frame errors: %lu\r\n",
			(unsigned long)commands->stats.frame_error_count);
	}

	if (commands->stats.last_reported_protobuf_decode_error_count !=
		commands->stats.protobuf_decode_error_count)
	{
		commands->stats.last_reported_protobuf_decode_error_count =
			commands->stats.protobuf_decode_error_count;
		std::printf("[USART1] Protobuf decode errors: %lu\r\n",
			(unsigned long)commands->stats.protobuf_decode_error_count);
	}
}
