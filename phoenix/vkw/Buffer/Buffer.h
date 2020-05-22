#pragma once

#include "../Allocator/Allocator.h"
#include "../vulkan.h"

#include "../Vertex.h"
#include "../VulkanResource.h"
#include <ltl/traits.h>

namespace phx {
template <typename T, VkBufferUsageFlags _bufferUsage,
          VmaMemoryUsage _memoryType>
class Buffer final {
public:
  static constexpr auto type = ltl::type_v<T>;
  static constexpr auto bufferUsage = vk::BufferUsageFlags(_bufferUsage);
  static constexpr auto memoryType = _memoryType;

  Buffer(Allocator &allocator, vk::DeviceSize size) noexcept
      : m_allocator{allocator}, //
        m_capacity{size} {
    vk::BufferCreateInfo infoBuffer;
    infoBuffer.size = size * sizeof(T);
    infoBuffer.usage = bufferUsage;
    infoBuffer.sharingMode = vk::SharingMode::eExclusive;

    ltl::tie(m_buffer, m_block) =
        allocator.allocateBuffer(infoBuffer, memoryType);
  }

  Buffer(Buffer &&buffer) noexcept
      : m_allocator(buffer.m_allocator),  //
        m_buffer(buffer.m_buffer),        //
        m_size{std::move(buffer.m_size)}, //
        m_capacity{buffer.m_capacity},    //
        m_block(buffer.m_block) {
    buffer.m_block = std::nullopt;
    buffer.m_capacity = 0;
    buffer.m_buffer = vk::Buffer();
  }

  Buffer &operator=(Buffer buffer) noexcept {
    deallocate();
    m_buffer = buffer.m_buffer;
    m_block = buffer.m_block;
    m_capacity = buffer.m_capacity;
    m_size = std::move(buffer.m_size);
    buffer.m_block = std::nullopt;
    buffer.m_capacity = 0;
    buffer.m_buffer = vk::Buffer();
    return *this;
  }

  void clear() noexcept {
    assert(m_block);
    m_block->clear();
  }

  auto getHandle() const noexcept {
    assert(m_block);
    return m_buffer;
  }

  ~Buffer() { deallocate(); }

  Buffer &operator<<(T x) noexcept {
    assert(m_block);
    constexpr auto has_types = IS_VALID((x), x.types);

    if_constexpr(has_types(x)) {
      auto types = x.types;
      types([this, &x](auto... types) {
        ((*this << static_cast<const decltype_t(types) &>(x)), ...);
      });
    }
    else {
      m_block->push_back(x);
    }
    ++(*m_size);
    return *this;
  }

  vk::DeviceSize size() const noexcept { return *m_size; }

  void setSize(vk::DeviceSize size) const noexcept {
    assert(size <= capacity());
    *m_size = size;
  }

  vk::DeviceSize sizeInBytes() const noexcept { return size() * sizeof(T); }

  vk::DeviceSize capacity() const noexcept { return m_capacity; }

  const vk::DeviceSize *sizePtr() const noexcept { return m_size.get(); }

  auto ptr() {
    assert(m_block);
    return reinterpret_cast<T *>(m_block->ptr());
  };

private:
  template <typename _T> Buffer &operator<<(_T x) noexcept {
    m_block->push_back(x);
    return *this;
  }

  void deallocate() {
    if (m_block)
      m_allocator.deallocateBuffer(m_buffer, *m_block);
  }

private:
  Allocator &m_allocator;
  vk::Buffer m_buffer;
  std::unique_ptr<vk::DeviceSize> m_size = std::make_unique<vk::DeviceSize>(0);
  vk::DeviceSize m_capacity;
  std::optional<AllocatorBlock> m_block;
};

template <typename T, VkBufferUsageFlags usage>
using CpuBuffer = Buffer<T, usage, VMA_MEMORY_USAGE_CPU_COPY>;

template <typename T, VkBufferUsageFlags usage>
using GpuBuffer = Buffer<T, usage, VMA_MEMORY_USAGE_GPU_ONLY>;

template <typename T>
using StagingBuffer = CpuBuffer<T, VK_BUFFER_USAGE_TRANSFER_SRC_BIT>;

template <typename T>
using VertexBuffer = GpuBuffer<T, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT>;

template <typename T>
using IndexBuffer = GpuBuffer<T, VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                     VK_BUFFER_USAGE_TRANSFER_DST_BIT>;

template <typename T>
using CpuUniformBuffer = CpuBuffer<T, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT>;

MAKE_IS_VULKAN_RESOURCE(vk::Buffer, is_buffer, IsBuffer);

template <vk::BufferUsageFlagBits usage, typename B>
constexpr auto doesBufferSupport(B &b) noexcept {
  typed_static_assert_msg(is_buffer(b), "b must be a buffer");
  return ltl::bool_v<(std::decay_t<B>::bufferUsage & usage) == usage>;
}

template <typename B, requires_f(IsBuffer<B>)> auto getDescriptorInfo(B &b) {
  return vk::DescriptorBufferInfo(b.getHandle(), 0, b.sizeInBytes());
}

} // namespace phx
