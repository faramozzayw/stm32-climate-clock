#ifndef INC_UTILS_BYTE_RING_BUFFER_HPP_
#define INC_UTILS_BYTE_RING_BUFFER_HPP_

#include <cstddef>
#include <cstdint>

namespace climate_clock
{
/**
 * @brief Single-producer/single-consumer byte ring buffer.
 *
 * One slot remains unused to distinguish full from empty. The producer owns
 * head and the consumer owns tail, allowing an ISR producer and main-loop
 * consumer without disabling interrupts.
 */
class ByteRingBuffer
{
  public:
	ByteRingBuffer(
		std::uint8_t *storage,
		std::size_t capacity,
		volatile std::uint16_t &head,
		volatile std::uint16_t &tail);

	void reset();
	[[nodiscard]] bool push(std::uint8_t byte);
	[[nodiscard]] bool pop(std::uint8_t &byte);
	[[nodiscard]] bool is_empty() const;

  private:
	[[nodiscard]] std::uint16_t next_index(std::uint16_t index) const;

	std::uint8_t *storage_;
	std::size_t capacity_;
	volatile std::uint16_t &head_;
	volatile std::uint16_t &tail_;
};
} // namespace climate_clock

#endif /* INC_UTILS_BYTE_RING_BUFFER_HPP_ */
