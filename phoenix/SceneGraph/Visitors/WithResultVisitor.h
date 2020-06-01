#pragma once

#include "NodeVisitor.h"
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
} // namespace phx
