#pragma once
#include "Buffer.h"

namespace phx {
template <typename T, VkBufferUsageFlags _bufferUsage> class BufferRef {
public:
  static constexpr auto type = ltl::type_v<T>;
  static constexpr auto bufferUsage = vk::BufferUsageFlags(_bufferUsage);

  BufferRef(const BufferRef &) = default;

  template <typename T, VkBufferUsageFlags usage>
  BufferRef(const BufferRef<T, usage> &buffer) {
    typed_static_assert(buffer.type == type);
    typed_static_assert((buffer.bufferUsage & bufferUsage) == bufferUsage);
    m_buffer = buffer.m_buffer;
    m_size = buffer.m_size;
    m_capacity = buffer.m_capacity;
  }

  template <typename T, VkBufferUsageFlags usage, VmaMemoryUsage memUsage>
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

  auto size() const noexcept { return *m_size; }
  auto capacity() const noexcept { return m_capacity; }

  vk::DeviceSize sizeInBytes() const noexcept { return size() * sizeof(T); }

private:
  vk::Buffer m_buffer;
  const vk::DeviceSize *m_size;
  vk::DeviceSize m_capacity;
};

template <typename T>
using VertexBufferRef = BufferRef<T, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT>;

template <typename T>
using IndexBufferRef = BufferRef<T, VK_BUFFER_USAGE_INDEX_BUFFER_BIT>;

template <typename T>
using UniformBufferRef = BufferRef<T, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT>;

template <typename T>
using StorageBufferRef = BufferRef<T, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT>;

} // namespace phx
