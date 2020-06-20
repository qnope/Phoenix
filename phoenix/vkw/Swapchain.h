#pragma once
#include "../constant.h"
#include "Device.h"
#include "Framebuffer.h"
#include "Image/Image.h"
#include "Surface.h"
#include "VulkanResource.h"
#include "vulkan.h"
#include <ltl/ltl.h>

namespace phx {
struct NoFormatAvailableException {};
class Swapchain final : public VulkanResource<vk::UniqueSwapchainKHR> {
  public:
    Swapchain(Device &device, Surface &surface, Width width, Height height);

    vk::Format getImageFormat() const noexcept;
    uint32_t getImageCount() const noexcept;

    vk::AttachmentDescription getAttachmentDescription() const noexcept;
    void generateFramebuffer(vk::RenderPass renderpass) noexcept;

    const Framebuffer<vk::ImageView> &getFramebuffer(uint32_t index) const noexcept;
    const std::vector<Framebuffer<vk::ImageView>> &getFramebuffers() const noexcept;

  private:
    using SwapchainImage = Image<vk::ImageType::e2D, vk::Format::eB8G8R8A8Srgb, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT>;
    using SwapchainImageView =
        ImageView<vk::ImageViewType::e2D, vk::Format::eB8G8R8A8Srgb, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT>;

    vk::Device m_device;
    vk::Extent3D m_extent;
    uint32_t m_imageCount;

    std::vector<ltl::tuple_t<SwapchainImage, SwapchainImageView>> m_swapchainImages;
    std::vector<Framebuffer<vk::ImageView>> m_framebuffers;

    vk::SurfaceFormatKHR m_surfaceFormat;
};
} // namespace phx
