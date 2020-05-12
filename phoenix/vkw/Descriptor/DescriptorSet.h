#pragma once

#include "DescriptorBinding.h"
#include <ltl/functional.h>

namespace phx {
template <typename SetLayout> class DescriptorSet {
public:
  DescriptorSet(vk::Device device, vk::DescriptorSet set,
                decltype_t(SetLayout::type_list) value_list) noexcept
      : m_set{set} {
    ltl::zip_with(ltl::curry(lift(writeDescriptorSet), device, set),
                  SetLayout::offset_list, SetLayout::binding_list, value_list);
  }

  auto getHandle() const noexcept { return m_set; }

private:
  vk::DescriptorSet m_set;
};
} // namespace phx
