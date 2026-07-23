#include "telemetry.h"

#include "command_receiver/uart_frame.h"
#include "device.pb.h"
#include "pb_encode.h"

#define TELEMETRY_UART_TIMEOUT_MS 100U

bool telemetry_send_temperature(
	UART_HandleTypeDef *uart,
	int16_t temperature_celsius)
{
	if (uart == NULL)
	{
		return false;
	}

	device_DeviceTelemetry telemetry = device_DeviceTelemetry_init_zero;
	uint8_t payload[device_DeviceTelemetry_size];
	uint8_t frame[UART_FRAME_MAX_SIZE];
	pb_ostream_t stream;
	uint16_t frame_length;

	telemetry.current_temp = temperature_celsius;
	stream = pb_ostream_from_buffer(payload, sizeof(payload));

	if (!pb_encode(&stream, device_DeviceTelemetry_fields, &telemetry))
	{
		return false;
	}

	if (!uart_frame_encode(
			payload,
			(uint16_t)stream.bytes_written,
			frame,
			sizeof(frame),
			&frame_length))
	{
		return false;
	}

	return HAL_UART_Transmit(
			   uart,
			   frame,
			   frame_length,
			   TELEMETRY_UART_TIMEOUT_MS) == HAL_OK;
}
