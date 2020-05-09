#pragma once

#include "VulkanResource.h"
#include "vulkan.h"

#include <ltl/Tuple.h>

namespace phx {
template <typename... SetLayouts>
class PipelineLayout : public VulkanResource<vk::UniquePipelineLayout> {
public:
  PipelineLayout(vk::Device device, SetLayouts... setLayouts)
      : descriptorSetLayouts{setLayouts.getHandle()...}, //
        m_setLayouts{std::move(setLayouts)...} {

    vk::PipelineLayoutCreateInfo info;
    info.setLayoutCount = uint32_t(descriptorSetLayouts.size());
    info.pSetLayouts = descriptorSetLayouts.data();
    m_handle = device.createPipelineLayoutUnique(info);
  }

public:
  static constexpr auto descriptor_types = ltl::type_list_v<SetLayouts...>;

private:
  std::array<vk::DescriptorSetLayout, descriptor_types.length.value>
      descriptorSetLayouts;
  ltl::tuple_t<SetLayouts...> m_setLayouts;
};
} // namespace phx
