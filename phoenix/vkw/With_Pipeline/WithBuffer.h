#pragma once

#include "../vulkan.h"
#include <ltl/algos.h>
#include <ltl/traits.h>

namespace phx {
template <typename Binding, typename Type, std::size_t AttributeCount>
struct BindingDescription;

template <int Binding, typename Type, std::size_t AttributeCount>
struct BindingDescription<ltl::number_t<Binding>, ltl::type_t<Type>,
                          AttributeCount> {
  template <typename... Attributes>
  BindingDescription(ltl::number_t<Binding>, ltl::type_t<Type>, uint32_t stride,
                     Attributes... attributes)
      : stride{stride}, //
        attributes{attributes...} {}
  uint32_t stride;
  vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex;
  std::array<vk::VertexInputAttributeDescription, AttributeCount> attributes;

  static constexpr auto type = ltl::type_v<Type>;
  static constexpr auto binding = number_v<Binding>;
};

template <typename Binding, typename Type, typename... Ts>
BindingDescription(Binding, Type, uint32_t, Ts...)
    ->BindingDescription<Binding, Type, sizeof...(Ts)>;

template <typename... Bindings> struct WithBindingDescriptions;

template <int... Bindings, typename... Types, std::size_t... AttributeCounts>
struct WithBindingDescriptions<BindingDescription<
    ltl::number_t<Bindings>, ltl::type_t<Types>, AttributeCounts>...> {
  WithBindingDescriptions(
      BindingDescription<ltl::number_t<Bindings>, ltl::type_t<Types>,
                         AttributeCounts>... descriptions)
      : bindings{vk::VertexInputBindingDescription(descriptions.binding.value,
                                                   descriptions.stride,
                                                   descriptions.inputRate)...} {
    ((ltl::copy(descriptions.attributes, std::back_inserter(attributes))), ...);
  }

  std::array<vk::VertexInputBindingDescription, sizeof...(Bindings)> bindings;
  std::vector<vk::VertexInputAttributeDescription> attributes;
};

template <typename... Bindings>
WithBindingDescriptions(Bindings...)->WithBindingDescriptions<Bindings...>;

LTL_MAKE_IS_KIND(WithBindingDescriptions, is_with_binding_descriptions,
                 IsWithBindingDescriptions, typename);

} // namespace phx
