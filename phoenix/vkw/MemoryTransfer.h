#pragma once

#include "Barriers.h"
#include "CommandPool.h"
#include "Device.h"
#include "Queue.h"

#include "Buffer/Buffer.h"
#include "Image/Image.h"
#include <ltl/Range/DefaultView.h>
#include <ltl/algos.h>
#include <ltl/ltl.h>
#include <ltl/operator.h>

namespace phx {

struct BufferTransferBarrier {
  vk::PipelineStageFlags dstStage;
  vk::AccessFlags dstMask;
};

struct ImageTransferBarrier {
  vk::PipelineStageFlags dstStage;
  vk::AccessFlags dstMask;
  vk::ImageLayout newLayout;
  vk::ImageSubresourceRange range;
};

class MemoryTransfer {
  template <typename T> class MemoryTransferToBuffer {
  public:
    MemoryTransferToBuffer(MemoryTransfer &memoryTransfer, T &buffer)
        : memoryTransfer{memoryTransfer}, buffer{buffer} {}

    MemoryTransferToBuffer &operator<<(Barrier barrier) const noexcept {
      memoryTransfer.applyBarrier(barrier);
      return *this;
    }

    MemoryTransferToBuffer &operator<<(BufferTransferBarrier barrier) noexcept {
      memoryTransfer.applyMemoryBarrier(barrier.dstStage, barrier.dstMask);
      return *this;
    }

    template <typename _T>
    MemoryTransferToBuffer &operator<<(const _T &bufferToCopy) noexcept {
      vk::BufferCopy range(0, buffer.sizeInBytes(), bufferToCopy.sizeInBytes());
      buffer.setSize(buffer.size() + bufferToCopy.size());
      memoryTransfer.copyBufferRange(bufferToCopy, buffer, range);
      return *this;
    }

    ~MemoryTransferToBuffer() { memoryTransfer.flush(); }

  private:
    MemoryTransfer &memoryTransfer;
    T &buffer;
  };

  template <typename T> class MemoryTransferToImage {
  public:
    MemoryTransferToImage(MemoryTransfer &memoryTransfer, T &image) noexcept
        : memoryTransfer{memoryTransfer}, image{image} {}

    MemoryTransferToImage &operator<<(Barrier barrier) noexcept {
      memoryTransfer.applyBarrier(barrier);
      return *this;
    }

    template <typename _T, requires_f(IsBuffer<_T>)>
    MemoryTransferToImage &operator<<(const _T &bufferToCopy) noexcept {
      memoryTransfer.copyBufferToImage(bufferToCopy, image);
      return *this;
    }

    ~MemoryTransferToImage() { memoryTransfer.flush(); }

  private:
    MemoryTransfer &memoryTransfer;
    T &image;
  };

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

  template <typename B, typename I>
  void copyBufferToImage(const B &buffer, const I &image) {
    vk::BufferImageCopy region{};

    region.imageExtent = image.getExtent();
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.layerCount = 1;
    region.imageSubresource.aspectMask = image.aspectMask;
    region.imageSubresource.baseArrayLayer = 0;

    copyBufferToImage(buffer, image, region);
  }

  template <typename B, typename I>
  void copyBufferToImage(const B &buffer, const I &image,
                         vk::BufferImageCopy range) {
    typed_static_assert_msg(
        doesBufferSupport<vk::BufferUsageFlagBits::eTransferSrc>(buffer),
        "Src Buffer must be a transferable source");

    typed_static_assert_msg(
        doesImageSupport<vk::ImageUsageFlagBits::eTransferDst>(image),
        "image must be a transferable destination");

    getCurrentCommandBuffer().copyBufferToImage(
        buffer.getHandle(), image.getHandle(),
        vk::ImageLayout::eTransferDstOptimal, range);
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

  void applyBarrier(Barrier barrier) {
    ::phx::applyBarrier(getCurrentCommandBuffer(), barrier);
  }

  void applyMemoryBarrier(vk::PipelineStageFlags dstStage,
                          vk::AccessFlags dstMasks) {
    MemoryBarrier barrier;
    barrier.srcStage = vk::PipelineStageFlagBits::eTransfer;
    barrier.dstStage = dstStage;
    barrier.srcMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstMask = dstMasks;
    ::phx::applyBarrier(getCurrentCommandBuffer(), barrier);
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

  template <typename Image, requires_f(IsImage<Image>)>
  auto to(const Image &image) {
    return MemoryTransferToImage{*this, image};
  }

  template <typename T, auto... enums> auto to(Buffer<T, enums...> &buffer) {
    return MemoryTransferToBuffer{*this, buffer};
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
