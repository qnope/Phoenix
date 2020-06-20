#pragma once

#include "../VulkanResource.h"
#include "DescriptorBinding.h"
#include <ltl/functional.h>

#include <typeindex>

namespace phx {
class DescriptorSet {
  public:
    template <typename SetLayout>
    DescriptorSet(vk::Device device, ltl::type_t<SetLayout>, vk::DescriptorSet set,
                  decltype_t(SetLayout::type_list) value_list) noexcept :
        m_layoutType{typeid(SetLayout)}, //
        m_set{set} {
        ltl::zip_with(ltl::curry(lift(writeDescriptorSet), device, set), SetLayout::offset_list,
                      SetLayout::binding_list, value_list);
    }

    auto getHandle() const noexcept { return m_set; }

    auto layoutType() const noexcept { return m_layoutType; }

  private:
    std::type_index m_layoutType;
    vk::DescriptorSet m_set;
};

} // namespace phx
