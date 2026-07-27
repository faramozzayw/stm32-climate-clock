#include "utils/byte_ring_buffer.h"

#include <stddef.h>

static bool byte_ring_buffer_is_initialized(const byte_ring_buffer_t *buffer)
{
	return (buffer != NULL) &&
		   (buffer->storage != NULL) &&
		   (buffer->capacity >= 2U);
}

bool byte_ring_buffer_init(
	byte_ring_buffer_t *buffer,
	uint8_t *storage,
	uint16_t capacity)
{
	if ((buffer == NULL) || (storage == NULL) || (capacity < 2U))
	{
		return false;
	}

	buffer->storage = storage;
	buffer->capacity = capacity;
	buffer->head = 0U;
	buffer->tail = 0U;
	return true;
}

bool byte_ring_buffer_push(byte_ring_buffer_t *buffer, uint8_t byte)
{
	uint16_t next_head;

	if (!byte_ring_buffer_is_initialized(buffer))
	{
		return false;
	}

	next_head = (uint16_t)(buffer->head + 1U);
	if (next_head >= buffer->capacity)
	{
		next_head = 0U;
	}

	if (next_head == buffer->tail)
	{
		return false;
	}

	buffer->storage[buffer->head] = byte;
	buffer->head = next_head;
	return true;
}

bool byte_ring_buffer_pop(byte_ring_buffer_t *buffer, uint8_t *byte)
{
	uint16_t next_tail;

	if (!byte_ring_buffer_is_initialized(buffer) || (byte == NULL) ||
		(buffer->tail == buffer->head))
	{
		return false;
	}

	*byte = buffer->storage[buffer->tail];
	next_tail = (uint16_t)(buffer->tail + 1U);
	if (next_tail >= buffer->capacity)
	{
		next_tail = 0U;
	}

	buffer->tail = next_tail;
	return true;
}

bool byte_ring_buffer_is_empty(const byte_ring_buffer_t *buffer)
{
	return !byte_ring_buffer_is_initialized(buffer) ||
		   (buffer->tail == buffer->head);
}
