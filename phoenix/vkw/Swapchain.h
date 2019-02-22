#pragma once
#include "../constant.h"
#include "Device.h"
#include "Framebuffer.h"
#include "Image.h"
#include "Surface.h"
#include "VulkanResource.h"
#include "vulkan.hpp"
#include <ltl/ltl.h>

namespace phx {
struct NoFormatAvailableException {};
class Swapchain final : public VulkanResource<vk::UniqueSwapchainKHR> {
public:
  Swapchain(Device &device, Surface &surface, Width width, Height height);

  vk::Format getImageFormat() const noexcept;

  vk::AttachmentDescription getAttachmentDescription() const noexcept;
  void generateFramebuffer(vk::RenderPass renderpass) noexcept;

  vk::Framebuffer getFramebuffer(uint32_t index) const noexcept;

private:
  using SwapchainImage = Image<ColorAttachmentUsageBit>;
  using SwapchainImageView = ImageView<ColorAttachmentUsageBit>;

  vk::Device m_device;
  vk::Extent3D m_extent;

  std::vector<ltl::tuple_t<SwapchainImage, SwapchainImageView>> m_swapchainImages;
  std::vector<Framebuffer> m_framebuffers;

  vk::SurfaceFormatKHR m_surfaceFormat;
};
} // namespace phx
