#pragma once
#include "Subpass.h"
#include "../vulkan.h"

#include <ltl/ltl.h>

namespace phx {
template <typename... Ns>
constexpr auto buildNoDepthStencilNoInputColorsSubpassDescription(Ns... ns) {
    typed_static_assert((ltl::is_number_t(ns) && ... && true_v));
    constexpr ltl::tuple_t outputs{AttachmentReference{ns, vk::ImageLayout::eColorAttachmentOptimal}...};

    return SubpassDescription{outputs, ltl::tuple_t{}, ltl::tuple_t{}, ltl::tuple_t{}};
}

inline auto buildPresentationDependency() {
    vk::SubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    return dependency;
}
} // namespace phx
