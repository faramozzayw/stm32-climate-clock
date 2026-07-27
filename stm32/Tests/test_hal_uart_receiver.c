#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "unity.h"
#include "platform/hal_uart_receiver.h"

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

static void test_adapter_initializes_and_starts_receive(void)
{
	hal_uart_receiver_t adapter;
	UART_HandleTypeDef uart = {0};
	uart_command_receiver_t receiver;

	reset_mocks();
	TEST_ASSERT_TRUE(!hal_uart_receiver_init(NULL, &uart, &receiver));
	TEST_ASSERT_TRUE(!hal_uart_receiver_init(&adapter, NULL, &receiver));
	TEST_ASSERT_TRUE(!hal_uart_receiver_init(&adapter, &uart, NULL));
	TEST_ASSERT_TRUE(hal_uart_receiver_init(&adapter, &uart, &receiver));
	TEST_ASSERT_TRUE(adapter.uart == &uart);
	TEST_ASSERT_TRUE(adapter.receiver == &receiver);
	TEST_ASSERT_TRUE(adapter.rearm_error_count == 0U);

	TEST_ASSERT_TRUE(hal_uart_receiver_start(&adapter));
	TEST_ASSERT_TRUE(mock_receive_call_count == 1U);
	TEST_ASSERT_TRUE(mock_receive_uart == &uart);
	TEST_ASSERT_TRUE(mock_receive_data == &adapter.pending_byte);
	TEST_ASSERT_TRUE(mock_receive_size == 1U);
}

static void test_adapter_routes_only_its_uart_and_rearms(void)
{
	hal_uart_receiver_t adapter;
	UART_HandleTypeDef uart = {0};
	UART_HandleTypeDef other_uart = {0};
	uart_command_receiver_t receiver;

	reset_mocks();
	TEST_ASSERT_TRUE(hal_uart_receiver_init(&adapter, &uart, &receiver));
	TEST_ASSERT_TRUE(hal_uart_receiver_start(&adapter));
	adapter.pending_byte = 0x5AU;

	hal_uart_receiver_on_complete(&adapter, &other_uart);
	TEST_ASSERT_TRUE(pushed_byte_count == 0U);
	TEST_ASSERT_TRUE(mock_receive_call_count == 1U);

	hal_uart_receiver_on_complete(&adapter, &uart);
	TEST_ASSERT_TRUE(pushed_byte_count == 1U);
	TEST_ASSERT_TRUE(pushed_receiver == &receiver);
	TEST_ASSERT_TRUE(pushed_byte == 0x5AU);
	TEST_ASSERT_TRUE(mock_receive_call_count == 2U);
	TEST_ASSERT_TRUE(mock_receive_data == &adapter.pending_byte);
}

static void test_adapter_tracks_deferred_rearm_failures(void)
{
	hal_uart_receiver_t adapter;
	UART_HandleTypeDef uart = {0};
	uart_command_receiver_t receiver;

	reset_mocks();
	TEST_ASSERT_TRUE(hal_uart_receiver_init(&adapter, &uart, &receiver));
	mock_receive_result = HAL_ERROR;
	TEST_ASSERT_TRUE(!hal_uart_receiver_start(&adapter));
	TEST_ASSERT_TRUE(adapter.rearm_error_count == 1U);

	adapter.pending_byte = 0x11U;
	hal_uart_receiver_on_complete(&adapter, &uart);
	TEST_ASSERT_TRUE(pushed_byte_count == 1U);
	TEST_ASSERT_TRUE(adapter.rearm_error_count == 2U);
	TEST_ASSERT_TRUE(adapter.last_reported_rearm_error_count == 0U);

	hal_uart_receiver_report_errors(&adapter);
	TEST_ASSERT_TRUE(adapter.last_reported_rearm_error_count == 2U);
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
	RUN_TEST(test_adapter_initializes_and_starts_receive);
	RUN_TEST(test_adapter_routes_only_its_uart_and_rearms);
	RUN_TEST(test_adapter_tracks_deferred_rearm_failures);
	return UNITY_END();
}
