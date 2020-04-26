#pragma once

#include <cassert>
#include <cstddef>
#include <cstring>
#include <vk_mem_alloc.h>

namespace phx {
class AllocatorBlock {
public:
  AllocatorBlock() = default;
  AllocatorBlock(VmaAllocation allocation, void *ptr, std::size_t size)
      : m_allocation{allocation}, m_maxSize{size}, m_mappedMemory{ptr} {}

  void clear() { m_currentOffset = 0; }

  template <typename T> void push_back(T t) {
    assert(sizeof(T) + m_currentOffset <= m_maxSize);
    char *ptr = reinterpret_cast<char *>(m_mappedMemory) + m_currentOffset;
    m_currentOffset += sizeof(T);
    std::memcpy(ptr, &t, sizeof(T));
  }

  VmaAllocation getAllocation() const noexcept { return m_allocation; }

private:
  VmaAllocation m_allocation;
  std::size_t m_currentOffset = 0;
  std::size_t m_maxSize;
  void *m_mappedMemory;
};
} // namespace phx
