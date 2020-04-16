#pragma once
#include "VulkanResource.h"
#include "vulkan.h"
#include <ltl/tuple_algos.h>

namespace phx {

template <typename... AttachmentTypes>
class Framebuffer : public VulkanResource<vk::UniqueFramebuffer> {
  static constexpr ltl::type_list_t<AttachmentTypes...> attachment_types{};
  typed_static_assert_msg(
      ltl::all_of_type(attachment_types,
                       ltl::is_type(ltl::type_v<vk::ImageView>)),
      "All Attachment types must be vk::ImageView");

public:
  static constexpr auto number_attachments = attachment_types.length;

  Framebuffer(vk::Device device, vk::RenderPass renderPass, uint32_t width,
              uint32_t height,
              ltl::tuple_t<AttachmentTypes...> attachments) noexcept
      : m_width{width}, m_height{height} {
    vk::FramebufferCreateInfo info;
    auto buildArray = [](auto... xs) {
      return std::array<vk::ImageView, number_attachments.value>{xs...};
    };
    auto attachmentArray = attachments(buildArray);
    info.renderPass = renderPass;
    info.attachmentCount = static_cast<uint32_t>(attachmentArray.size());
    info.pAttachments = attachmentArray.data();
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

LTL_MAKE_IS_KIND(Framebuffer, is_framebuffer, IsFramebuffer, typename);
} // namespace phx
