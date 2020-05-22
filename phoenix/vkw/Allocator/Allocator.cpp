#define VMA_IMPLEMENTATION
#include "Allocator.h"

namespace phx {

Allocator::Allocator(vk::Instance instance, vk::Device device,
                     vk::PhysicalDevice physicalDevice)
    : m_device{device} {
  VmaAllocatorCreateInfo info{};
  info.device = device;
  info.instance = instance;
  info.physicalDevice = physicalDevice;
  vmaCreateAllocator(&info, &m_allocator);
}

vk::Device Allocator::device() const noexcept { return m_device; }

ltl::tuple_t<vk::Image, VmaAllocation>
Allocator::allocateImage(vk::ImageCreateInfo _infoImage, VmaMemoryUsage usage) {
  VkImage image{};

  VmaAllocationCreateInfo infoAlloc{};
  infoAlloc.usage = usage;

  VmaAllocation allocation{};
  VkImageCreateInfo infoImage = _infoImage;
  vmaCreateImage(m_allocator, &infoImage, &infoAlloc, &image, &allocation,
                 nullptr);
  return {image, allocation};
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
                         reinterpret_cast<char *>(blockInfo.pMappedData),
                         _infoBuffer.size)};
}

void Allocator::deallocateImage(vk::Image image, VmaAllocation allocation) {
  vmaDestroyImage(m_allocator, image, allocation);
}

void Allocator::deallocateBuffer(vk::Buffer buffer, AllocatorBlock block) {
  vmaDestroyBuffer(m_allocator, buffer, block.getAllocation());
}

Allocator::~Allocator() { vmaDestroyAllocator(m_allocator); }

} // namespace phx
