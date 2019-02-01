#pragma once

#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {

class Queue final {
public:
  Queue(vk::Queue queue, uint32_t indexFamily) noexcept;

  uint32_t getIndexFamily() const noexcept;

private:
  vk::Queue m_queue;
  uint32_t m_indexFamily;
};
} // namespace phx
