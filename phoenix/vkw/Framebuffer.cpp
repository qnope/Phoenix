#include "Framebuffer.h"

namespace phx {

Framebuffer::Framebuffer(vk::Device device, vk::RenderPass renderPass, uint32_t width,
                         uint32_t height, const std::vector<vk::ImageView> &attachments) {
  vk::FramebufferCreateInfo info;
  info.renderPass = renderPass;
  info.attachmentCount = attachments.size();
  info.pAttachments = attachments.data();
  info.width = width;
  info.height = height;
  m_handle = device.createFramebufferUnique(info);
}

} // namespace phx
