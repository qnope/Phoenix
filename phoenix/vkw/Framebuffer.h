#pragma once
#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {
class Framebuffer : public VulkanResource<vk::UniqueFramebuffer> {
public:
  Framebuffer(vk::Device device, vk::RenderPass renderPass, uint32_t width,
              uint32_t height, const std::vector<vk::ImageView> &attachments);

private:
};
} // namespace phx
