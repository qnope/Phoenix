#include "Queue.h"

namespace phx {

Queue::Queue(vk::Queue queue, uint32_t indexFamily) noexcept
    : m_queue{queue}, m_indexFamily{indexFamily} {}

uint32_t Queue::getIndexFamily() const noexcept { return m_indexFamily; }

} // namespace phx
