#ifndef INC_BYTE_RING_BUFFER_H_
#define INC_BYTE_RING_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Single-producer/single-consumer byte ring buffer.
 *
 * One slot remains unused to distinguish full from empty. The producer owns
 * head and the consumer owns tail, allowing an ISR producer and main-loop
 * consumer without disabling interrupts.
 */
typedef struct
{
	volatile uint8_t *storage;
	uint16_t capacity;
	volatile uint16_t head;
	volatile uint16_t tail;
} byte_ring_buffer_t;

bool byte_ring_buffer_init(
	byte_ring_buffer_t *buffer,
	uint8_t *storage,
	uint16_t capacity);

bool byte_ring_buffer_push(byte_ring_buffer_t *buffer, uint8_t byte);
bool byte_ring_buffer_pop(byte_ring_buffer_t *buffer, uint8_t *byte);
bool byte_ring_buffer_is_empty(const byte_ring_buffer_t *buffer);

#endif /* INC_BYTE_RING_BUFFER_H_ */
