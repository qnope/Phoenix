#pragma once

#include "VulkanResource.h"
#include "vulkan.hpp"

#include "Fence.h"

namespace phx {

struct WaitSemaphore {
  vk::Semaphore semaphore;
  vk::PipelineStageFlags stages;
};

class Queue final {
public:
  Queue(vk::Queue queue, uint32_t indexFamily) noexcept;

  vk::Queue getHandle() const noexcept { return m_queue; }

  uint32_t getIndexFamily() const noexcept;

  void present(vk::Semaphore waitSemaphore, vk::SwapchainKHR swapChain,
               uint32_t imageIndice) const noexcept;

  void push(vk::CommandBuffer cmdBuffer) noexcept;
  void push(WaitSemaphore waitSemaphore) noexcept;
  void push(vk::Semaphore signalSemaphore) noexcept;

  void next() noexcept;
  void flush(vk::Fence = vk::Fence()) noexcept;

private:
  void populateSubmits() noexcept;

private:
  vk::Queue m_queue;
  uint32_t m_indexFamily;

  std::vector<uint32_t> m_commandBufferBySubmitNumbers;
  std::vector<uint32_t> m_waitSemaphoreBySubmitNumbers;
  std::vector<uint32_t> m_signalSemaphoreBySubmitNumbers;

  std::vector<vk::SubmitInfo> m_submits;
  std::vector<vk::CommandBuffer> m_commandBuffers;
  std::vector<vk::Semaphore> m_waitSemaphores;
  std::vector<vk::PipelineStageFlags> m_waitStages;
  std::vector<vk::Semaphore> m_signalSemaphores;
};

struct Flush {
  Flush() = default;
  Flush(const Fence &fence) : fence{fence.getHandle()} {}

  Flush operator()(const Fence &fence) const noexcept { return Flush{fence}; }

  vk::Fence fence;
};

inline Flush flush;

inline Queue &operator<<(Queue &queue, Flush flush) noexcept {
  queue.flush(flush.fence);
  return queue;
}

template <typename T> inline Queue &operator<<(Queue &queue, T t) noexcept {
  queue.push(t);
  return queue;
}

} // namespace phx
