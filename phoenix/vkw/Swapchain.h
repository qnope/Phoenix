#pragma once
#include "../constant.h"
#include "Device.h"
#include "Image.h"
#include "Surface.h"
#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {
struct NoFormatAvailableException {};
class Swapchain final : public VulkanResource<vk::UniqueSwapchainKHR> {
public:
  Swapchain(Device &device, Surface &surface, Width width, Height height);

private:
  using SwapchainImage = Image<colorAttachmentUsageBit()>;
  std::vector<SwapchainImage> m_swapchainImages;
};
} // namespace phx
