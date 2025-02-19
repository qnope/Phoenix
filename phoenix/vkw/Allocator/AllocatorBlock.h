#pragma once

#include "../vulkan.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <vk_mem_alloc.h>

namespace phx {
class AllocatorBlock {
  public:
    AllocatorBlock() = default;
    AllocatorBlock(VmaAllocation allocation, void *ptr, std::size_t size) :
        m_allocation{allocation}, m_capacity{size}, m_mappedMemory{ptr} {}

    void clear() { m_currentOffset = 0; }

    template <typename T>
    void push_back(T t) {
        assert(sizeof(T) + m_currentOffset <= m_capacity);
        char *ptr = reinterpret_cast<char *>(m_mappedMemory) + m_currentOffset;
        m_currentOffset += sizeof(T);
        std::memcpy(ptr, &t, sizeof(T));
    }

    VmaAllocation getAllocation() const noexcept { return m_allocation; }

    auto ptr() { return m_mappedMemory; }

  private:
    VmaAllocation m_allocation;
    vk::DeviceSize m_currentOffset = 0;
    vk::DeviceSize m_capacity;
    void *m_mappedMemory;
};
} // namespace phx
