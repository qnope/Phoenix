#pragma once

#include "../Allocator/Allocator.h"
#include "../vulkan.h"

#include "../Vertex.h"

namespace phx {
template <typename T, vk::BufferUsageFlagBits _bufferType,
          VmaMemoryUsage _memoryType>
class Buffer final {
public:
  using value_type = T;
  static constexpr auto bufferType = _bufferType;
  static constexpr auto memoryType = _memoryType;

  Buffer(vk::Device device, Allocator &allocator, vk::DeviceSize size)
      : m_allocator{allocator} {
    vk::BufferCreateInfo infoBuffer;
    infoBuffer.size = size;
    infoBuffer.usage = bufferType;
    infoBuffer.sharingMode = vk::SharingMode::eExclusive;

    ltl::tie(m_buffer, m_block) =
        allocator.allocateBuffer(infoBuffer, memoryType);
  }

  void clear() { m_block.clear(); }

  auto getHandle() const { return m_buffer; }

  ~Buffer() { m_allocator.deallocateBuffer(m_buffer, m_block); }

  template <typename T> friend Buffer &operator<<(Buffer &buffer, T x) {
    buffer.m_block.push_back(x);
    return buffer;
  }

  template <typename... Ts>
  friend Buffer &operator<<(Buffer &buffer, Vertex<Ts...> x) {
    ((buffer << static_cast<const Ts &>(x)), ...);
    return buffer;
  }

private:
  Allocator &m_allocator;
  vk::Buffer m_buffer;
  AllocatorBlock m_block;
};

template <typename T>
using CpuVertexBuffer = Buffer<T, vk::BufferUsageFlagBits::eVertexBuffer,
                               VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY>;
} // namespace phx
