#pragma once

#include "../vulkan.h"
#include <ltl/Tuple.h>
#include <ltl/ltl.h>
#include <ltl/traits.h>

namespace phx {
namespace dynamic_state {
struct DynamicState {
protected:
  DynamicState() = default;
};
struct DynamicViewport : DynamicState {
  static constexpr auto state = vk::DynamicState::eViewport;
};
struct DynamicScissor : DynamicState {
  static constexpr auto state = vk::DynamicState::eScissor;
};
constexpr DynamicScissor dynamic_scissor{};
constexpr DynamicViewport dynamic_viewport{};
} // namespace dynamic_state

template <typename... DynamicStates> struct WithDynamicStates {
  static constexpr ltl::type_list_t<DynamicStates...> dynamic_state_types{};
  static constexpr auto numberDynamicStates = dynamic_state_types.length;

  auto hasDynamicViewport() {
    using namespace ltl;
    return contains_type(dynamic_state_types,
                         type_v<dynamic_state::DynamicViewport>);
  }

  auto hasDynamicScissor() {
    using namespace ltl;
    return contains_type(dynamic_state_types,
                         type_v<dynamic_state::DynamicScissor>);
  }

  auto getDynamicStates() { return std::array{DynamicStates::state...}; }

  constexpr WithDynamicStates(DynamicStates...) {
    using namespace ltl;
    typed_static_assert_msg(numberDynamicStates > 0_n,
                            "If you specify a WithDynamicStates, you must have "
                            "at least one dynamic state");

    typed_static_assert_msg(
        all_of_type(dynamic_state_types,
                    is_derived_from(type_v<dynamic_state::DynamicState>)),
        "All dynamic states must be DynamicStates values");

    typed_static_assert_msg(
        count_type(dynamic_state_types,
                   type_v<dynamic_state::DynamicViewport>) <= 1_n &&
            count_type(dynamic_state_types,
                       type_v<dynamic_state::DynamicScissor>) <= 1_n,
        "You cannot specify several times the same dynamic state");
  }
};

LTL_MAKE_IS_KIND(WithDynamicStates, is_with_dynamic_states, IsWithDynamicStates,
                 typename);

} // namespace phx
