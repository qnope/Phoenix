#pragma once

#include "vulkan.hpp"

namespace phx {

class Queue final {
public:
  Queue(vk::Queue queue) noexcept;

private:
  vk::Queue m_queue;
};
} // namespace phx
