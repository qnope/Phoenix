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
          bindings[index].toDescriptorBinding(binding_association[index][0_n]);
    });

    vk::DescriptorSetLayoutCreateInfo info;
    info.bindingCount = binding_count.value;
    info.pBindings = bindingArray.data();
    m_handle = device.createDescriptorSetLayoutUnique(info);
  }

  static constexpr auto binding_count = number_v<sizeof...(Bindings)>;

private:
  static constexpr auto make_association() {
    using namespace ltl;
    auto binding_types = type_list_v<Bindings...>;
    auto indexer = build_index_sequence(binding_count);

    auto computer = [binding_types](auto indice) {
      auto sub_indexer = build_index_sequence(indice);
      auto binding_index = sub_indexer([binding_types](auto... is) {
        return (0_n + ... + (decltype_t(binding_types[is])::count));
      });
      return ltl::tuple_t{binding_index, binding_types[indice]};
    };
    return ltl::transform_type(indexer, computer);
  }

public:
  static constexpr auto binding_association = decltype(make_association()){};
};
} // namespace phx
