#pragma once

#include "../vulkan.h"
#include <ltl/traits.h>

namespace phx {

template <typename Binding> struct BindingDescription;

template <int _binding> struct BindingDescription<ltl::number_t<_binding>> {
  uint32_t stride{};
  vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex;
  static constexpr auto binding = number_v<_binding>;
};

// template <> class AttributeDescription {};

template <typename... Bindings> struct WithBindingDescriptions;

template <typename... Bindings>
struct WithBindingDescriptions<BindingDescription<Bindings>...> {
  WithBindingDescriptions(BindingDescription<Bindings>... descriptions)
      : descriptions{vk::VertexInputBindingDescription(
            descriptions.binding.value, descriptions.stride,
            descriptions.inputRate)...} {}

  std::array<vk::VertexInputBindingDescription, sizeof...(Bindings)>
      descriptions;
};

template <typename... Bindings>
WithBindingDescriptions(Bindings...)->WithBindingDescriptions<Bindings...>;

LTL_MAKE_IS_KIND(WithBindingDescriptions, is_with_binding_descriptions,
                 IsWithBindingDescriptions, typename);

} // namespace phx
