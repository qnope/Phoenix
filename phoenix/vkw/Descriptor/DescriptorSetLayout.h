#pragma once

#include "../VulkanResource.h"
#include "DescriptorBinding.h"
#include <ltl/operator.h>
#include <ltl/tuple_algos.h>

namespace phx {
template <typename... Bindings>
class DescriptorSetLayout : public VulkanResource<vk::UniqueDescriptorSetLayout> {
  public:
    DescriptorSetLayout(vk::Device device) {
        auto zipped = ltl::zip_type(binding_list, offset_list);
        auto bindingArray =
            zipped([](auto... zipped) { return std::array{zipped[0_n].toDescriptorBinding(zipped[1_n])...}; });

        vk::DescriptorSetLayoutCreateInfo info;
        info.bindingCount = binding_count.value;
        info.pBindings = bindingArray.data();
        m_handle = device.createDescriptorSetLayoutUnique(info);
    }

    static constexpr auto binding_list = ltl::tuple_t<Bindings...>{};
    static constexpr auto binding_count = binding_list.length;

  private:
    static constexpr auto make_offset_list() {
        return ltl::scanl(_((x, y), x + y.count), 0_n, binding_list).pop_back();
    }

  public:
    static constexpr auto offset_list = decltype(make_offset_list()){};
    static constexpr auto type_list = ltl::type_v<ltl::tuple_t<decltype_t(Bindings::types)...>>;
};

} // namespace phx
