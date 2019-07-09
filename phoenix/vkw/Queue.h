#pragma once

#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {

struct WaitSemaphore {
  vk::Semaphore semaphore;
  vk::PipelineStageFlagBits stages;
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

struct Flush {};
constexpr Flush flush;

inline Queue &operator<<(Queue &queue, Flush) noexcept {
  queue.flush();
  return queue;
}

template <typename T> inline Queue &operator<<(Queue &queue, T t) noexcept {
  queue.push(t);
  return queue;
}

} // namespace phx
