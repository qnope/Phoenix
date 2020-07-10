#pragma once
#include "VulkanResource.h"
#include "vulkan.h"
#include <ltl/tuple_algos.h>

namespace phx {

template <std::size_t imageCount>
class Framebuffer : public VulkanResource<vk::UniqueFramebuffer> {
  public:
    static constexpr auto number_attachments = number_v<imageCount>;

    template <typename... ImageViews>
    Framebuffer(vk::Device device, vk::RenderPass renderPass, uint32_t width, uint32_t height,
                const ImageViews &... attachments) noexcept :
        m_width{width},
        m_height{height} {
        auto attachmentsArray = std::array<vk::ImageView, imageCount>{attachments.getHandle()...};

        vk::FramebufferCreateInfo info;
        info.renderPass = renderPass;
        info.attachmentCount = static_cast<uint32_t>(attachmentsArray.size());
        info.pAttachments = attachmentsArray.data();
        info.width = width;
        info.height = height;
        info.layers = 1;
        m_handle = device.createFramebufferUnique(info);
    }

    uint32_t getWidth() const noexcept { return m_width; }
    uint32_t getHeight() const noexcept { return m_height; }

  private:
    uint32_t m_width;
    uint32_t m_height;
};

template <typename... ImageViews>
Framebuffer(vk::Device, vk::RenderPass, uint32_t, uint32_t, const ImageViews &...)->Framebuffer<sizeof...(ImageViews)>;

} // namespace phx
