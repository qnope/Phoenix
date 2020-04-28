#pragma once
#include "Buffer.h"

namespace phx {
template <typename T, vk::BufferUsageFlagBits _bufferUsage> class BufferRef {
public:
  static constexpr auto type = ltl::type_v<T>;
  static constexpr auto bufferUsage = _bufferUsage;

  BufferRef(const BufferRef &) = default;

  template <typename T, vk::BufferUsageFlagBits usage, VmaMemoryUsage memUsage>
  BufferRef(Buffer<T, usage, memUsage> &buffer) {
    typed_static_assert_msg(buffer.type == type,
                            "Buffer must be of the good type");
    static_assert((buffer.bufferUsage & bufferUsage) == bufferUsage,
                  "Buffer must have the good usage");
    m_buffer = buffer.getHandle();
    m_offset = 0;
    m_size = buffer.size();
  }

  auto getHandle() const noexcept { return m_buffer; }

private:
  vk::Buffer m_buffer;
  vk::DeviceSize m_offset;
  vk::DeviceSize m_size;
};

template <typename T>
using VertexBufferRef = BufferRef<T, vk::BufferUsageFlagBits::eVertexBuffer>;

} // namespace phx
