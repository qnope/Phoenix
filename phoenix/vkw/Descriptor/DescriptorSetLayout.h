#pragma once

#include "../VulkanResource.h"
#include "DescriptorBinding.h"
#include <ltl/tuple_algos.h>

namespace phx {
template <typename... Bindings>
class DescriptorSetLayout
    : public VulkanResource<vk::UniqueDescriptorSetLayout> {
public:
  DescriptorSetLayout(vk::Device device, Bindings... _bindings) {
    auto bindings = ltl::tuple_t{_bindings...};
    auto indexer = ltl::build_index_sequence(binding_count);

    std::array<vk::DescriptorSetLayoutBinding, binding_count.value>
        bindingArray;

    ltl::for_each(indexer, [&](auto index) {
      bindingArray[index.value] =
          bindings[index].toDescriptorBinding(index_list[index]);
    });

    vk::DescriptorSetLayoutCreateInfo info;
    info.bindingCount = binding_count.value;
    info.pBindings = bindingArray.data();
    m_handle = device.createDescriptorSetLayoutUnique(info);
  }

  static constexpr auto binding_list = ltl::tuple_t<Bindings...>{};
  static constexpr auto binding_count = binding_list.length;

private:
  static constexpr auto make_index_list() {
    using namespace ltl;
    auto indexer = build_index_sequence(binding_count);

    auto computer = [](auto indice) {
      auto sub_indexer = build_index_sequence(indice);
      auto binding_index = sub_indexer(
          [](auto... is) { return (0_n + ... + (binding_list[is].count)); });
      return binding_index;
    };
    return ltl::transform_type(indexer, computer);
  }

public:
  static constexpr auto index_list = make_index_list();
  static constexpr auto type_list =
      ltl::type_v<ltl::tuple_t<decltype_t(Bindings::types)...>>;
};

} // namespace phx
