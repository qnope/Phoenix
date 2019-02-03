#pragma once
#include "../constant.h"
#include "Device.h"
#include "Image.h"
#include "Surface.h"
#include "VulkanResource.h"
#include "vulkan.hpp"

#include "ltl/tuple.h"

namespace phx {
struct NoFormatAvailableException {};
class Swapchain final : public VulkanResource<vk::UniqueSwapchainKHR> {
public:
  Swapchain(Device &device, Surface &surface, Width width, Height height);

private:
  using SwapchainImage = Image<colorAttachmentUsageBit()>;
  using SwapchainImageView = ImageView<colorAttachmentUsageBit()>;
  std::vector<ltl::tuple_t<SwapchainImage, SwapchainImageView>>
      m_swapchainImages;
};
} // namespace phx
