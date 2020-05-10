#pragma once

#include "DescriptorBinding.h"

namespace phx {
template <typename SetLayout> class DescriptorSet {
public:
  DescriptorSet(vk::Device device, vk::DescriptorSet set,
                decltype_t(SetLayout::type_list) values) noexcept
      : m_set{set} {

    values([set, device](auto... values) {
      constexpr auto indices = SetLayout::index_list;
      indices([set, device, values...](auto... indices) {
        constexpr auto bindings = SetLayout::binding_list;
        bindings([set, device, indices..., values...](auto... bindings) {
          (writeDescriptorSet(device, set, indices, bindings, values), ...);
        });
      });
    });
  }

  auto getHandle() const noexcept { return m_set; }

private:
  vk::DescriptorSet m_set;
};
} // namespace phx
