#pragma once

#include "../Allocator/Allocator.h"
#include "../vulkan.h"

#include "../Vertex.h"
#include <ltl/traits.h>

namespace phx {
template <typename T, vk::BufferUsageFlagBits _bufferUsage,
          VmaMemoryUsage _memoryType>
class Buffer final {
public:
  static constexpr auto type = ltl::type_v<T>;
  static constexpr auto bufferUsage = _bufferUsage;
  static constexpr auto memoryType = _memoryType;

  Buffer(vk::Device device, Allocator &allocator, vk::DeviceSize size)
      : m_allocator{allocator} {
    vk::BufferCreateInfo infoBuffer;
    infoBuffer.size = size;
    infoBuffer.usage = bufferUsage;
    infoBuffer.sharingMode = vk::SharingMode::eExclusive;

    ltl::tie(m_buffer, m_block) =
        allocator.allocateBuffer(infoBuffer, memoryType);
  }

  Buffer(Buffer &&buffer) noexcept
      : m_allocator(buffer.m_allocator), m_buffer(buffer.m_buffer),
        m_block(buffer.m_block) {
    buffer.m_block = std::nullopt;
  }

  Buffer &operator=(Buffer &&buffer) noexcept {
    deallocate();
    m_buffer = buffer.m_buffer;
    m_block = buffer.m_block;
    buffer.m_block = std::nullopt;
    return *this;
  }

  void clear() {
    assert(m_block);
    m_block->clear();
  }

  auto getHandle() const {
    assert(m_block);
    return m_buffer;
  }

  ~Buffer() { deallocate(); }

  template <typename T> friend Buffer &operator<<(Buffer &buffer, T x) {
    assert(buffer.m_block);
    buffer.m_block->push_back(x);
    return buffer;
  }

  template <typename... Ts>
  friend Buffer &operator<<(Buffer &buffer, Vertex<Ts...> x) {
    ((buffer << static_cast<const Ts &>(x)), ...);
    return buffer;
  }

  vk::DeviceSize size() const {
    assert(m_block);
    return m_block->size();
  }

private:
  void deallocate() {
    if (m_block)
      m_allocator.deallocateBuffer(m_buffer, *m_block);
  }

private:
  Allocator &m_allocator;
  vk::Buffer m_buffer;
  std::optional<AllocatorBlock> m_block;
};

template <typename T>
using CpuVertexBuffer = Buffer<T, vk::BufferUsageFlagBits::eVertexBuffer,
                               VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY>;
} // namespace phx
