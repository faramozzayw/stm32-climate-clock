#include "utils/byte_ring_buffer.hpp"

namespace climate_clock
{
ByteRingBuffer::ByteRingBuffer(
	std::uint8_t *storage,
	std::size_t capacity,
	volatile std::uint16_t &head,
	volatile std::uint16_t &tail)
	: storage_(storage),
	  capacity_(capacity),
	  head_(head),
	  tail_(tail)
{
}

void ByteRingBuffer::reset()
{
	head_ = 0U;
	tail_ = 0U;
}

bool ByteRingBuffer::push(std::uint8_t byte)
{
	const std::uint16_t next_head = next_index(head_);
	if (next_head == tail_)
	{
		return false;
	}

	storage_[head_] = byte;
	head_ = next_head;
	return true;
}

bool ByteRingBuffer::pop(std::uint8_t &byte)
{
	if (is_empty())
	{
		return false;
	}

	byte = storage_[tail_];
	tail_ = next_index(tail_);
	return true;
}

bool ByteRingBuffer::is_empty() const
{
	return tail_ == head_;
}

std::uint16_t ByteRingBuffer::next_index(std::uint16_t index) const
{
	const std::uint16_t next = static_cast<std::uint16_t>(index + 1U);
	return next < capacity_ ? next : 0U;
}
} // namespace climate_clock
