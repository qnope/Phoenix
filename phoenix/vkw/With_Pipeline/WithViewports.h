#pragma once

#include "../vulkan.h"
#include <array>
#include <ltl/ltl.h>

namespace phx {
namespace viewport {
struct Viewport {
protected:
  Viewport() = default;
};

struct StaticViewport : Viewport {
  StaticViewport(Width width, Height height)
      : m_viewport{0.0f,
                   0.0f,
                   static_cast<float>(width.get()),
                   static_cast<float>(height.get()),
                   0.0f,
                   1.0f} {}

  vk::Viewport m_viewport;
};

struct DynamicViewport : Viewport {};
constexpr DynamicViewport dynamic_viewport;
} // namespace viewport

template <typename... Viewports> class WithViewports {
  static constexpr ltl::type_list_t<Viewports...> viewport_types{};

  static constexpr auto numberDynamicViewports =
      ltl::count_type(viewport_types, ltl::type_v<viewport::DynamicViewport>);
  static constexpr auto numberStaticViewports =
      ltl::count_type(viewport_types, ltl::type_v<viewport::StaticViewport>);

public:
  static constexpr auto numberViewports = viewport_types.length;
  static constexpr auto isDynamic = numberDynamicViewports > 0_n;
  static constexpr auto isStatic = !isDynamic;

  WithViewports(Viewports... viewports) {
    compileTimeCheck();

    if_constexpr(isStatic) m_viewports = std::array{viewports.m_viewport...};
  }

  auto getViewports() { return m_viewports; }

private:
  void compileTimeCheck() {
    using namespace ltl;
    typed_static_assert_msg(numberViewports > 0_n,
                            "You must have at least one viewport");
    typed_static_assert_msg(numberDynamicViewports == numberViewports ||
                                numberStaticViewports == numberViewports,
                            "You can not mix static and dynamic viewports.");
    typed_static_assert_msg(
        all_of_type(viewport_types,
                    is_derived_from(type_v<viewport::Viewport>)),
        "You must have only static or dynamic viewports within WithViewports.");
  }

private:
  std::array<vk::Viewport, numberViewports.value> m_viewports;
};

LTL_MAKE_IS_KIND(WithViewports, is_with_viewports, IsWithViewports, typename);

} // namespace phx
