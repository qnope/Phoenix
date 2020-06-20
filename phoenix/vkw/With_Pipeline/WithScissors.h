#pragma once
#include "../../constant.h"
#include "../vulkan.h"
#include <array>
#include <ltl/traits.h>
#include <ltl/tuple_algos.h>

namespace phx {
namespace scissor {
struct Scissor {
  protected:
    Scissor() = default;
};
struct StaticScissor : Scissor {
    StaticScissor(Width width, Height height) :
        m_scissor{vk::Offset2D{0, 0},
                  vk::Extent2D{static_cast<uint32_t>(width.get()), static_cast<uint32_t>(height.get())}} {}

    vk::Rect2D m_scissor;
};

struct DynamicScissor : Scissor {};
constexpr DynamicScissor dynamic_scissor;
} // namespace scissor

template <typename... Scissors>
class WithScissors {
    static constexpr ltl::type_list_t<Scissors...> scissor_types{};

    static constexpr auto numberDynamicScissors = ltl::count_type(scissor_types, ltl::type_v<scissor::DynamicScissor>);
    static constexpr auto numberStaticScissors = ltl::count_type(scissor_types, ltl::type_v<scissor::StaticScissor>);

  public:
    static constexpr auto numberScissors = scissor_types.length;
    static constexpr auto isDynamic = numberDynamicScissors > 0_n;
    static constexpr auto isStatic = !isDynamic;

    WithScissors(Scissors... scissors) {
        compileTimeCheck();
        if_constexpr(isStatic) m_scissors = std::array{scissors.m_scissor...};
    }

    auto getScissors() { return m_scissors; }

  private:
    void compileTimeCheck() {
        using namespace ltl;
        typed_static_assert_msg(numberScissors > 0_n, "You must have at least one scissor.");
        typed_static_assert_msg(numberDynamicScissors == numberScissors || numberStaticScissors == numberScissors,
                                "You can not mix static and dynamic scissors.");
        typed_static_assert_msg(all_of_type(scissor_types, is_derived_from(type_v<scissor::Scissor>)),
                                "You must have only static or dynamic scissors within WithScissors. ");
    }

  private:
    std::array<vk::Rect2D, numberScissors.value> m_scissors;
};

LTL_MAKE_IS_KIND(WithScissors, is_with_scissors, IsWithScissors, typename);

} // namespace phx
