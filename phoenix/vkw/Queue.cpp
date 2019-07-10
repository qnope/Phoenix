#include "Queue.h"
#include <ltl/ltl.h>

namespace phx {

Queue::Queue(vk::Queue queue, uint32_t indexFamily) noexcept
    : m_queue{queue}, m_indexFamily{indexFamily} {
  next();
}

uint32_t Queue::getIndexFamily() const noexcept { return m_indexFamily; }

void Queue::present(vk::Semaphore waitSemaphore, vk::SwapchainKHR swapChain,
                    uint32_t imageIndice) const noexcept {
  vk::PresentInfoKHR info;
  info.swapchainCount = info.waitSemaphoreCount = 1;
  info.pSwapchains = &swapChain;
  info.pWaitSemaphores = &waitSemaphore;
  info.pImageIndices = &imageIndice;
  getHandle().presentKHR(info);
}

void Queue::push(vk::CommandBuffer cmdBuffer) noexcept {
  ++m_commandBufferBySubmitNumbers.back();
  m_commandBuffers.emplace_back(cmdBuffer);
}

void Queue::push(WaitSemaphore waitSemaphore) noexcept {
  ++m_waitSemaphoreBySubmitNumbers.back();
  m_waitStages.emplace_back(waitSemaphore.stages);
  m_waitSemaphores.emplace_back(waitSemaphore.semaphore);
}

void Queue::push(vk::Semaphore signalSemaphore) noexcept {
  ++m_signalSemaphoreBySubmitNumbers.back();
  m_signalSemaphores.emplace_back(signalSemaphore);
}

void Queue::next() noexcept {
  m_commandBufferBySubmitNumbers.emplace_back(0);
  m_waitSemaphoreBySubmitNumbers.emplace_back(0);
  m_signalSemaphoreBySubmitNumbers.emplace_back(0);
}

void Queue::flush(vk::Fence fence) noexcept {
  populateSubmits();
  getHandle().submit(m_submits, fence);
  m_submits.resize(0);
  m_waitStages.resize(0);
  m_commandBuffers.resize(0);
  m_waitSemaphores.resize(0);
  m_signalSemaphores.resize(0);

  m_commandBufferBySubmitNumbers.resize(0);
  m_waitSemaphoreBySubmitNumbers.resize(0);
  m_signalSemaphoreBySubmitNumbers.resize(0);
  next();
}

void Queue::populateSubmits() noexcept {
  int totalCmdNumber, totalWaitNumber, totalSignalNumber;
  totalCmdNumber = totalWaitNumber = totalSignalNumber = 0;
  for (auto [cmdNumber, waitNumber, signalNumber] :
       ltl::zip(m_commandBufferBySubmitNumbers, m_waitSemaphoreBySubmitNumbers,
                m_signalSemaphoreBySubmitNumbers)) {
    vk::SubmitInfo info;
    info.commandBufferCount = cmdNumber;
    info.waitSemaphoreCount = waitNumber;
    info.signalSemaphoreCount = signalNumber;

    info.pWaitDstStageMask = m_waitStages.data() + totalWaitNumber;
    info.pCommandBuffers = m_commandBuffers.data() + totalCmdNumber;
    info.pWaitSemaphores = m_waitSemaphores.data() + totalWaitNumber;
    info.pSignalSemaphores = m_signalSemaphores.data() + totalSignalNumber;

    totalCmdNumber += cmdNumber;
    totalWaitNumber += waitNumber;
    totalSignalNumber += signalNumber;
    m_submits.emplace_back(info);
  }
}

} // namespace phx
