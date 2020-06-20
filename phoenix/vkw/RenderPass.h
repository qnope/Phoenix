#pragma once

#include "Subpass.h"
#include "VulkanResource.h"

namespace phx {
template <typename... Args>
class RenderPass;

template <typename... AttachmentDescriptions, typename... Subpasses, typename... Dependencies>
class RenderPass<ltl::tuple_t<AttachmentDescriptions...>, ltl::tuple_t<Subpasses...>, ltl::tuple_t<Dependencies...>> :
    public VulkanResource<vk::UniqueRenderPass> {
    static constexpr ltl::type_list_t<AttachmentDescriptions...> attachment_types{};
    static constexpr ltl::type_list_t<Subpasses...> subpass_types{};
    static constexpr ltl::type_list_t<Dependencies...> depency_types{};

  public:
    static constexpr auto number_attachments = attachment_types.length;
    static constexpr auto number_subpasses = subpass_types.length;
    static constexpr auto number_dependencies = depency_types.length;

    RenderPass(vk::Device device, ltl::tuple_t<AttachmentDescriptions...> attachments,
               ltl::tuple_t<Subpasses...> subpasses, ltl::tuple_t<Dependencies...> dependencies) {
        compileTimeCheck();
        auto toAttachmentDescriptions = [](auto... xs) {
            return std::array<vk::AttachmentDescription, number_attachments.value>{xs...};
        };

        auto toSubpassDescriptions = [](auto &... xs) {
            return std::array<vk::SubpassDescription, number_subpasses.value>{xs.getSubpassDescription()...};
        };

        auto toSubpassDependencies = [subpasses](auto... xs) {
            return std::array<vk::SubpassDependency, number_dependencies.value>{xs...};
        };

        auto toClearValues = [](auto... xs) {
            auto toClearValue = [](vk::AttachmentDescription desc) -> vk::ClearValue {
                switch (desc.format) {
                case vk::Format::eD16Unorm:
                case vk::Format::eD16UnormS8Uint:
                case vk::Format::eD24UnormS8Uint:
                case vk::Format::eD32Sfloat:
                case vk::Format::eD32SfloatS8Uint:
                    return vk::ClearDepthStencilValue{1.0f, 0};
                default:
                    return vk::ClearColorValue{std::array{0.0f, 0.0f, 0.0f, 1.0f}};
                }
            };
            return std::array<vk::ClearValue, number_attachments.value>{toClearValue(xs)...};
        };

        const auto attachmentArray = attachments(toAttachmentDescriptions);
        const auto subpassArray = subpasses(toSubpassDescriptions);
        const auto dependencyArray = dependencies(toSubpassDependencies);

        vk::RenderPassCreateInfo info;
        info.attachmentCount = static_cast<uint32_t>(attachmentArray.size());
        info.subpassCount = static_cast<uint32_t>(subpassArray.size());
        info.dependencyCount = static_cast<uint32_t>(dependencyArray.size());

        info.pAttachments = attachmentArray.data();
        info.pSubpasses = subpassArray.data();
        info.pDependencies = dependencyArray.data();
        m_handle = device.createRenderPassUnique(info);
        m_clearValues = attachments(toClearValues);
    }

    auto getClearValues() const noexcept { return m_clearValues; }

  private:
    void compileTimeCheck() {
        using namespace ltl;

        typed_static_assert_msg(all_of_type(attachment_types, is_type(type_v<vk::AttachmentDescription>)),
                                "AttachmentDescriptions must be vk::AttachmentDescriptions");
        typed_static_assert_msg(all_of_type(subpass_types, is_subpass_description), "Subpasses must be Subpasses");
        typed_static_assert_msg(all_of_type(depency_types, is_type(type_v<vk::SubpassDependency>)),
                                "Dependencies must be SubpassDependencies");
        typed_static_assert_msg(!subpass_types.isEmpty, "RenderPass must have at least one subpass");

        auto isSubpassCompatible = [](auto subpass) {
            return decltype_t(subpass)::getMaxAttachmentIndex() < RenderPass::number_attachments;
        };

        typed_static_assert_msg(all_of_type(subpass_types, isSubpassCompatible),
                                "Subpass cannot have out of bounds attachments");
    }

  private:
    std::array<vk::ClearValue, number_attachments.value> m_clearValues;
};

LTL_MAKE_IS_KIND(RenderPass, is_render_pass, IsRenderPass, typename);

} // namespace phx
