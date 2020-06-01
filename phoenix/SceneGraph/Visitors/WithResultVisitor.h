#pragma once

#include "NodeVisitor.h"
#include <ltl/traits.h>
#include <utility>

namespace phx {
struct AbstractWithResultVisitor : NodeVisitor {};

template <typename Result>
class WithResultVisitor : public AbstractWithResultVisitor {
public:
  Result takeResult() noexcept { return std::move(m_result); }

protected:
  Result m_result;
};

LTL_MAKE_IS_KIND(WithResultVisitor, is_with_result_visitor, IsWithResultVisitor,
                 typename);

} // namespace phx
