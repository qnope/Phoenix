#pragma once
#include "Buffer.h"

namespace phx {
template <typename T, vk::BufferUsageFlagBits _bufferUsage> class BufferRef {
public:
  static constexpr auto type = ltl::type_v<T>;
  static constexpr auto bufferUsage = _bufferUsage;

  BufferRef(const BufferRef &) = default;

  template <typename T, vk::BufferUsageFlagBits usage>
  BufferRef(const BufferRef<T, usage> &buffer) {
    typed_static_assert(buffer.type == type);
    typed_static_assert((buffer.bufferUsage & bufferUsage) == bufferUsage);
    m_buffer = buffer.m_buffer;
    m_size = buffer.m_size;
    m_capacity = buffer.m_capacity;
  }

  template <typename T, vk::BufferUsageFlagBits usage, VmaMemoryUsage memUsage>
  BufferRef(Buffer<T, usage, memUsage> &buffer) {
    typed_static_assert_msg(buffer.type == type,
                            "Buffer must be of the good type");
    static_assert((buffer.bufferUsage & bufferUsage) == bufferUsage,
                  "Buffer must have the good usage");
    m_buffer = buffer.getHandle();
    m_size = buffer.sizePtr();
    m_capacity = buffer.capacity();
  }

  auto getHandle() const noexcept { return m_buffer; }

  auto getSize() const noexcept { return *m_size; }
  auto getCapacity() const noexcept { return m_capacity; }

private:
  vk::Buffer m_buffer;
  vk::DeviceSize *m_size;
  vk::DeviceSize m_capacity;
};

template <typename T>
using VertexBufferRef = BufferRef<T, vk::BufferUsageFlagBits::eVertexBuffer>;

template <typename T>
using IndexBufferRef = BufferRef<T, vk::BufferUsageFlagBits::eIndexBuffer>;

} // namespace phx
