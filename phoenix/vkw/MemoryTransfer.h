#pragma once

#include "CommandPool.h"
#include "Device.h"
#include "Queue.h"

#include "Buffer/Buffer.h"
#include <ltl/Range/DefaultView.h>
#include <ltl/algos.h>
#include <ltl/ltl.h>
#include <ltl/operator.h>

namespace phx {
class MemoryTransfer {
public:
  MemoryTransfer(Device &device) noexcept
      : m_queue{device.getQueue()}, //
        m_commandPool{device.getHandle(), m_queue.getIndexFamily(), true,
                      false} {}

  void reset() {
    m_commandPool.reset();
    m_commandBuffers.clear();
  }

  template <typename B1, typename B2>
  void copyBuffer(const B1 &src, const B2 &dst) {
    vk::BufferCopy copy;
    copy.size = src.sizeInBytes();
    copyBufferRange(src, dst, copy);
  }

  template <typename B1, typename B2>
  void copyBufferRange(const B1 &src, const B2 &dst, vk::BufferCopy range) {
    typed_static_assert_msg(
        doesBufferSupport<vk::BufferUsageFlagBits::eTransferSrc>(src),
        "Src Buffer must be a transferable source");

    typed_static_assert_msg(
        doesBufferSupport<vk::BufferUsageFlagBits::eTransferDst>(dst),
        "Dst Buffer must be a transferable destination");

    typed_static_assert_msg(dst.type == src.type,
                            "Src and dst Buffer must be of the same type");

    assert(src.sizeInBytes() >= range.srcOffset + range.size);
    assert(dst.sizeInBytes() >= range.dstOffset + range.size);
    getCurrentCommandBuffer().copyBuffer(src.getHandle(), dst.getHandle(),
                                         range);
  }

  void flush() {
    if (auto it = ltl::find_if(m_commandBuffers, isLaunched)) {
      auto cmdBuffers = ltl::Range{*it, end(m_commandBuffers)} | ltl::values();
      auto endAndQueue = [this](auto x) {
        x.end();
        m_queue << x;
      };
      ltl::for_each(cmdBuffers, endAndQueue);
      m_commandBuffers.erase(*it, end(m_commandBuffers));
    }
  }

private:
  void allocateCommandBuffers() {
    auto toTuple = [](auto cmdBuffer) {
      return ltl::tuple_t{false, cmdBuffer};
    };

    m_commandBuffers = m_commandPool.allocateCommandBuffer(
                           vk::CommandBufferLevel::ePrimary, 10) |
                       ltl::map(toTuple) | ltl::to_vector;
  }

  void launchLastCommandBuffer() {
    if (m_commandBuffers.empty()) {
      allocateCommandBuffers();
    }
    vk::CommandBufferBeginInfo begin(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    m_commandBuffers.back()[0_n] = true;
    m_commandBuffers.back()[1_n].begin(begin);
  }

  vk::CommandBuffer getCurrentCommandBuffer() {
    if (auto lastLaunched = ltl::find_if_value(m_commandBuffers, isLaunched)) {
      return (*lastLaunched)[1_n];
    }

    launchLastCommandBuffer();
    return m_commandBuffers.back()[1_n];
  }

private:
  inline static const auto isLaunched = [](auto tuple) { return tuple[0_n]; };

private:
  Queue &m_queue;
  CommandPool m_commandPool;
  std::vector<ltl::tuple_t<bool, vk::CommandBuffer>> m_commandBuffers;
};
} // namespace phx
