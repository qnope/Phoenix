#pragma once

#include "VulkanResource.h"
#include "vulkan.h"

#include <ltl/Tuple.h>

namespace phx {
template <typename... SetLayouts>
class PipelineLayout : public VulkanResource<vk::UniquePipelineLayout> {
public:
  PipelineLayout(vk::Device device, const SetLayouts &... setLayouts) noexcept {
    std::array<vk::DescriptorSetLayout, layouts.length.value>
        descriptorSetLayouts = {setLayouts.getHandle()...};

    vk::PipelineLayoutCreateInfo info;
    info.setLayoutCount = uint32_t(descriptorSetLayouts.size());
    info.pSetLayouts = descriptorSetLayouts.data();

    m_handle = device.createPipelineLayoutUnique(info);
  }

public:
  static constexpr auto layouts = ltl::type_list_v<SetLayouts...>;
};

MAKE_IS_VULKAN_RESOURCE(vk::PipelineLayout, is_pipeline_layout,
                        IsPipelineLayout);
} // namespace phx
