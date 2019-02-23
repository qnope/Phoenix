#include "CommandPool.h"

namespace phx {
CommandPool::CommandPool(vk::Device device, uint32_t queueFamilyIndex, bool isTransient,
                         bool isResetable) noexcept
    : m_device{device} {
  vk::CommandPoolCreateInfo info;
  vk::CommandPoolCreateFlags flags;

  if (isTransient)
    flags |= vk::CommandPoolCreateFlagBits::eTransient;
  if (isResetable)
    flags |= vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

  info.flags = flags;
  info.queueFamilyIndex = queueFamilyIndex;

  m_handle = device.createCommandPoolUnique(info);
}

std::vector<vk::CommandBuffer>
CommandPool::allocateCommandBuffer(vk::CommandBufferLevel level, uint32_t count) const
    noexcept {
  vk::CommandBufferAllocateInfo info;
  info.commandBufferCount = count;
  info.commandPool = getHandle();
  info.level = level;
  return m_device.allocateCommandBuffers(info);
}
} // namespace phx
