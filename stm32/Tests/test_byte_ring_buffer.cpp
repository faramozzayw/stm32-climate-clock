extern "C"
{
#include "unity.h"
}

#include <iterator>

#include "utils/byte_ring_buffer.hpp"

using climate_clock::ByteRingBuffer;

static void test_byte_ring_buffer_preserves_fifo_order()
{
	std::uint8_t storage[4]{};
	volatile std::uint16_t head{};
	volatile std::uint16_t tail{};
	ByteRingBuffer buffer{storage, std::size(storage), head, tail};
	std::uint8_t byte{};

	buffer.reset();
	TEST_ASSERT_TRUE(buffer.is_empty());
	TEST_ASSERT_TRUE(buffer.push(10U));
	TEST_ASSERT_TRUE(buffer.push(20U));
	TEST_ASSERT_TRUE(buffer.pop(byte));
	TEST_ASSERT_EQUAL_UINT8(10U, byte);
	TEST_ASSERT_TRUE(buffer.pop(byte));
	TEST_ASSERT_EQUAL_UINT8(20U, byte);
	TEST_ASSERT_TRUE(buffer.is_empty());
	TEST_ASSERT_FALSE(buffer.pop(byte));
}

static void test_byte_ring_buffer_rejects_full_and_wraps()
{
	std::uint8_t storage[4]{};
	volatile std::uint16_t head{};
	volatile std::uint16_t tail{};
	ByteRingBuffer buffer{storage, std::size(storage), head, tail};
	std::uint8_t byte{};

	buffer.reset();
	TEST_ASSERT_TRUE(buffer.push(1U));
	TEST_ASSERT_TRUE(buffer.push(2U));
	TEST_ASSERT_TRUE(buffer.push(3U));
	TEST_ASSERT_FALSE(buffer.push(4U));

	TEST_ASSERT_TRUE(buffer.pop(byte));
	TEST_ASSERT_EQUAL_UINT8(1U, byte);
	TEST_ASSERT_TRUE(buffer.push(4U));

	TEST_ASSERT_TRUE(buffer.pop(byte));
	TEST_ASSERT_EQUAL_UINT8(2U, byte);
	TEST_ASSERT_TRUE(buffer.pop(byte));
	TEST_ASSERT_EQUAL_UINT8(3U, byte);
	TEST_ASSERT_TRUE(buffer.pop(byte));
	TEST_ASSERT_EQUAL_UINT8(4U, byte);
	TEST_ASSERT_TRUE(buffer.is_empty());
}

void setUp()
{
}

void tearDown()
{
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test_byte_ring_buffer_preserves_fifo_order);
	RUN_TEST(test_byte_ring_buffer_rejects_full_and_wraps);
	return UNITY_END();
}
