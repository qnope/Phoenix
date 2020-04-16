#pragma once

#include <ltl/StrongType.h>
#include <ltl/ltl.h>

namespace phx {
using Width = ltl::strong_type_t<unsigned int, struct WidthTag>;
using Height = ltl::strong_type_t<unsigned int, struct HeightTag>;
using WindowTitle = ltl::strong_type_t<std::string_view, struct WindowTitleTag>;
constexpr double pi = 3.14159265358979;

} // namespace phx
