#pragma once

#include "../vulkan.h"
#include "AllocatorBlock.h"
#include <ltl/Tuple.h>
#include <vk_mem_alloc.h>

namespace phx {

class Allocator {
public:
  Allocator(vk::Instance instance, vk::Device device,
            vk::PhysicalDevice physicalDevice);

  ltl::tuple_t<vk::Buffer, AllocatorBlock>
  allocateBuffer(vk::BufferCreateInfo infoBuffer, VmaMemoryUsage usage);

  void deallocateBuffer(vk::Buffer buffer, AllocatorBlock block);

  ~Allocator();

private:
  VmaAllocator m_allocator;
};
} // namespace phx
