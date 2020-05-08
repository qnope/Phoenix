#pragma once

#include "../vulkan.h"
#include <ltl/traits.h>

namespace phx {

template <vk::DescriptorType _type, uint32_t _count> struct DescriptorBinding {
  DescriptorBinding(vk::ShaderStageFlags stages) noexcept : stages{stages} {}

  template <int binding>
  auto toDescriptorBinding(ltl::number_t<binding>) const noexcept {
    return vk::DescriptorSetLayoutBinding(binding, type, count.value, stages);
  }

  static constexpr auto count = number_v<_count>;
  static constexpr auto type = _type;

  vk::ShaderStageFlags stages;
};
} // namespace phx
