#pragma once

#include "VulkanResource.h"
#include "vulkan.h"

#include "Descriptor/DescriptorSet.h"

#include <ltl/algos.h>
#include <ltl/tuple_algos.h>
#include <typeindex>

namespace phx {
class PipelineLayout : public VulkanResource<vk::UniquePipelineLayout> {
public:
  template <typename... SetLayouts>
  PipelineLayout(vk::Device device, const SetLayouts &... setLayouts) noexcept
      : m_layoutTypes{typeid(SetLayouts)...} {
    static constexpr auto layouts = ltl::type_list_v<SetLayouts...>;

    std::array<vk::DescriptorSetLayout, layouts.length.value>
        descriptorSetLayouts = {setLayouts.getHandle()...};

    vk::PipelineLayoutCreateInfo info;
    info.setLayoutCount = uint32_t(descriptorSetLayouts.size());
    info.pSetLayouts = descriptorSetLayouts.data();

    m_handle = device.createPipelineLayoutUnique(info);
  }

  void bind(vk::CommandBuffer cmdBuffer, vk::PipelineBindPoint bindPoint,
            uint32_t setIndex, DescriptorSet set) const noexcept {
    assert(setIndex < m_layoutTypes.size());
    assert(m_layoutTypes[setIndex] == set.layoutType());

    cmdBuffer.bindDescriptorSets(bindPoint, getHandle(), setIndex,
                                 set.getHandle(), {});
  }

  bool isCompatible(std::type_index layoutType) const noexcept {
    return ltl::contains(m_layoutTypes, layoutType);
  }

  uint32_t descriptorSetIndex(std::type_index layoutType) const noexcept {
    assert(isCompatible(layoutType));
    return uint32_t(std::distance(m_layoutTypes.begin(),
                                  *ltl::find(m_layoutTypes, layoutType)));
  }

public:
  std::vector<std::type_index> m_layoutTypes;
};

MAKE_IS_VULKAN_RESOURCE(vk::PipelineLayout, is_pipeline_layout,
                        IsPipelineLayout);
} // namespace phx
