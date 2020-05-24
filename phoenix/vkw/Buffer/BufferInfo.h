#pragma once

#include "BufferRef.h"

namespace phx {
template <typename T, VkBufferUsageFlags usages> struct BufferInfo {
  BufferRef<T, usages> buffer;
  std::size_t offset;
  std::size_t size;
};

using IndexBufferInfo = BufferInfo<uint32_t, VK_BUFFER_USAGE_INDEX_BUFFER_BIT>;
using VertexBufferInfo =
    BufferInfo<Textured2dVertex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT>;

} // namespace phx
