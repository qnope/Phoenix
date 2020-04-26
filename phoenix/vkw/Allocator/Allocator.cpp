#define VMA_IMPLEMENTATION
#include "Allocator.h"

namespace phx {

Allocator::Allocator(vk::Instance instance, vk::Device device,
                     vk::PhysicalDevice physicalDevice) {
  VmaAllocatorCreateInfo info{};
  info.device = device;
  info.instance = instance;
  info.physicalDevice = physicalDevice;
  vmaCreateAllocator(&info, &m_allocator);
}

ltl::tuple_t<vk::Buffer, AllocatorBlock>
Allocator::allocateBuffer(vk::BufferCreateInfo _infoBuffer,
                          VmaMemoryUsage usage) {
  VmaAllocationCreateInfo infoAlloc{};

  infoAlloc.usage = usage;
  infoAlloc.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  VkBuffer buffer{};
  VmaAllocation allocation{};
  VkBufferCreateInfo infoBuffer = _infoBuffer;
  VmaAllocationInfo blockInfo{};
  vmaCreateBuffer(m_allocator, &infoBuffer, &infoAlloc, &buffer, &allocation,
                  &blockInfo);

  return {buffer,
          AllocatorBlock(allocation,
                         reinterpret_cast<char *>(blockInfo.pMappedData) +
                             blockInfo.offset,
                         _infoBuffer.size)};
}

void Allocator::deallocateBuffer(vk::Buffer buffer, AllocatorBlock block) {
  vmaDestroyBuffer(m_allocator, buffer, block.getAllocation());
}

Allocator::~Allocator() { vmaDestroyAllocator(m_allocator); }

} // namespace phx
