#pragma once
#include "Subpass.h"
#include "vulkan.hpp"
#include <ltl/ltl.h>

namespace phx {

namespace detail {
template <typename... Ts> constexpr Subpass<Ts...> build_subpass(Ts... ts) {
  return {ts...};
}
} // namespace detail

template <typename... Ns> constexpr auto buildNoDepthStencilNoInputColors(Ns... ns) {
  typed_static_assert((ltl::is_number_t(ns) && ... && ltl::true_v));
  constexpr ltl::tuple_t outputs{
      AttachmentReference{ns, vk::ImageLayout::eColorAttachmentOptimal}...};

  return detail::build_subpass(outputs, ltl::tuple_t{}, ltl::tuple_t{}, ltl::tuple_t{});
}
} // namespace phx
