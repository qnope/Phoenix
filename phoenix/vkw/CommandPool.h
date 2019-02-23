#pragma once

#include "VulkanResource.h"
#include "vulkan.hpp"
namespace phx {
class CommandPool : public VulkanResource<vk::UniqueCommandPool> {
public:
  CommandPool(vk::Device device, uint32_t queueFamilyIndex, bool isTransient,
              bool isResetable) noexcept;

  std::vector<vk::CommandBuffer> allocateCommandBuffer(vk::CommandBufferLevel level,
                                                       uint32_t count) const noexcept;

private:
  vk::Device m_device;
};
} // namespace phx
