#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "platform/hal_uart_receiver.h"

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

static HAL_StatusTypeDef mock_receive_result = HAL_OK;
static uint32_t mock_receive_call_count;
static UART_HandleTypeDef *mock_receive_uart;
static uint8_t *mock_receive_data;
static uint16_t mock_receive_size;
static uint32_t pushed_byte_count;
static uart_command_receiver_t *pushed_receiver;
static uint8_t pushed_byte;

HAL_StatusTypeDef HAL_UART_Receive_IT(
	UART_HandleTypeDef *uart,
	uint8_t *data,
	uint16_t size)
{
	mock_receive_call_count++;
	mock_receive_uart = uart;
	mock_receive_data = data;
	mock_receive_size = size;
	return mock_receive_result;
}

void uart_command_receiver_push_byte(
	uart_command_receiver_t *receiver,
	uint8_t byte)
{
	pushed_byte_count++;
	pushed_receiver = receiver;
	pushed_byte = byte;
}

static void reset_mocks(void)
{
	mock_receive_result = HAL_OK;
	mock_receive_call_count = 0U;
	mock_receive_uart = NULL;
	mock_receive_data = NULL;
	mock_receive_size = 0U;
	pushed_byte_count = 0U;
	pushed_receiver = NULL;
	pushed_byte = 0U;
}

static bool test_adapter_initializes_and_starts_receive(void)
{
	hal_uart_receiver_t adapter;
	UART_HandleTypeDef uart = {0};
	uart_command_receiver_t receiver;

	reset_mocks();
	CHECK(!hal_uart_receiver_init(NULL, &uart, &receiver));
	CHECK(!hal_uart_receiver_init(&adapter, NULL, &receiver));
	CHECK(!hal_uart_receiver_init(&adapter, &uart, NULL));
	CHECK(hal_uart_receiver_init(&adapter, &uart, &receiver));
	CHECK(adapter.uart == &uart);
	CHECK(adapter.receiver == &receiver);
	CHECK(adapter.rearm_error_count == 0U);

	CHECK(hal_uart_receiver_start(&adapter));
	CHECK(mock_receive_call_count == 1U);
	CHECK(mock_receive_uart == &uart);
	CHECK(mock_receive_data == &adapter.pending_byte);
	CHECK(mock_receive_size == 1U);
	return true;
}

static bool test_adapter_routes_only_its_uart_and_rearms(void)
{
	hal_uart_receiver_t adapter;
	UART_HandleTypeDef uart = {0};
	UART_HandleTypeDef other_uart = {0};
	uart_command_receiver_t receiver;

	reset_mocks();
	CHECK(hal_uart_receiver_init(&adapter, &uart, &receiver));
	CHECK(hal_uart_receiver_start(&adapter));
	adapter.pending_byte = 0x5AU;

	hal_uart_receiver_on_complete(&adapter, &other_uart);
	CHECK(pushed_byte_count == 0U);
	CHECK(mock_receive_call_count == 1U);

	hal_uart_receiver_on_complete(&adapter, &uart);
	CHECK(pushed_byte_count == 1U);
	CHECK(pushed_receiver == &receiver);
	CHECK(pushed_byte == 0x5AU);
	CHECK(mock_receive_call_count == 2U);
	CHECK(mock_receive_data == &adapter.pending_byte);
	return true;
}

static bool test_adapter_tracks_deferred_rearm_failures(void)
{
	hal_uart_receiver_t adapter;
	UART_HandleTypeDef uart = {0};
	uart_command_receiver_t receiver;

	reset_mocks();
	CHECK(hal_uart_receiver_init(&adapter, &uart, &receiver));
	mock_receive_result = HAL_ERROR;
	CHECK(!hal_uart_receiver_start(&adapter));
	CHECK(adapter.rearm_error_count == 1U);

	adapter.pending_byte = 0x11U;
	hal_uart_receiver_on_complete(&adapter, &uart);
	CHECK(pushed_byte_count == 1U);
	CHECK(adapter.rearm_error_count == 2U);
	CHECK(adapter.last_reported_rearm_error_count == 0U);

	hal_uart_receiver_report_errors(&adapter);
	CHECK(adapter.last_reported_rearm_error_count == 2U);
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
		{"adapter initializes and starts receive", test_adapter_initializes_and_starts_receive},
		{"adapter routes its UART and rearms", test_adapter_routes_only_its_uart_and_rearms},
		{"adapter tracks deferred rearm failures", test_adapter_tracks_deferred_rearm_failures},
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
