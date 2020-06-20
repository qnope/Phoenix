#pragma once

#include "../vulkan.h"
#include "AllocatorBlock.h"
#include <ltl/Tuple.h>
#include <vk_mem_alloc.h>

namespace phx {

class Allocator {
  public:
    Allocator(vk::Instance instance, vk::Device device, vk::PhysicalDevice physicalDevice);

    vk::Device device() const noexcept;

    ltl::tuple_t<vk::Image, VmaAllocation> allocateImage(vk::ImageCreateInfo info, VmaMemoryUsage usage);

    ltl::tuple_t<vk::Buffer, AllocatorBlock> allocateBuffer(vk::BufferCreateInfo infoBuffer, VmaMemoryUsage usage);

    void deallocateImage(vk::Image image, VmaAllocation allocation);
    void deallocateBuffer(vk::Buffer buffer, AllocatorBlock block);

    ~Allocator();

  private:
    vk::Device m_device;
    VmaAllocator m_allocator;
};
} // namespace phx
