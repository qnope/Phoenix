#pragma once

#include "VulkanResource.h"
#include "vulkan.h"

#include "Descriptor/DescriptorSet.h"

#include <ltl/algos.h>
#include <ltl/tuple_algos.h>
#include <typeindex>

namespace phx {

inline struct with_layouts_t {
} with_layouts;

inline struct with_push_constants_t {
} with_push_constants;

template <uint32_t offset, uint32_t size, VkShaderStageFlags stages>
struct PushConstantRange {
  auto getRange() const {
    return vk::PushConstantRange{vk::ShaderStageFlags{stages}, offset, size};
  }
};

class PipelineLayout : public VulkanResource<vk::UniquePipelineLayout> {
public:
  template <typename... PushConstantRanges, typename... SetLayouts>
  PipelineLayout(vk::Device device, //
                 ltl::tuple_t<PushConstantRanges...> ranges,
                 ltl::tuple_t<const SetLayouts &...> setLayouts) noexcept
      : m_pushConstantRangeTypes{typeid(PushConstantRanges)...}, //
        m_layoutTypes{typeid(SetLayouts)...} {
    auto pushConstantRanges = ranges([](auto... range) {
      return std::array<vk::PushConstantRange, sizeof...(range)>{
          range.getRange()...};
    });

    auto layouts = setLayouts([](const auto &... layout) {
      return std::array<vk::DescriptorSetLayout, sizeof...(layout)>{
          layout.getHandle()...};
    });

    vk::PipelineLayoutCreateInfo info;
    info.pSetLayouts = layouts.data();
    info.setLayoutCount = uint32_t(layouts.size());

    info.pPushConstantRanges = pushConstantRanges.data();
    info.pushConstantRangeCount = uint32_t(pushConstantRanges.size());

    m_handle = device.createPipelineLayoutUnique(info);
  }

  PipelineLayout(vk::Device device) noexcept
      : PipelineLayout(device, ltl::tuple_t{}, ltl::tuple_t{}) {}

  template <typename... Layouts>
  PipelineLayout(vk::Device device, with_layouts_t,
                 const Layouts &... layouts) noexcept
      : PipelineLayout(device, ltl::tuple_t{},
                       ltl::tuple_t{std::cref(layouts)...}) {}

  template <typename... PushConstantRanges>
  PipelineLayout(vk::Device device, with_push_constants_t,
                 PushConstantRanges... ranges) noexcept
      : PipelineLayout(device, ltl::tuple_t{ranges...}, ltl::tuple_t{}) {}

  void bind(vk::CommandBuffer cmdBuffer, vk::PipelineBindPoint bindPoint,
            uint32_t setIndex, DescriptorSet set) const noexcept {
    assert(setIndex < m_layoutTypes.size());
    assert(m_layoutTypes[setIndex] == set.layoutType());

    cmdBuffer.bindDescriptorSets(bindPoint, getHandle(), setIndex,
                                 set.getHandle(), {});
  }

  bool hasLayout(std::type_index layoutType) const noexcept {
    return ltl::contains(m_layoutTypes, layoutType);
  }

  uint32_t descriptorSetIndex(std::type_index layoutType) const noexcept {
    assert(hasLayout(layoutType));
    return uint32_t(std::distance(m_layoutTypes.begin(),
                                  *ltl::find(m_layoutTypes, layoutType)));
  }

public:
  std::vector<std::type_index> m_pushConstantRangeTypes;
  std::vector<std::type_index> m_layoutTypes;
};

MAKE_IS_VULKAN_RESOURCE(vk::PipelineLayout, is_pipeline_layout,
                        IsPipelineLayout);
} // namespace phx
