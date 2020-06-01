#include "ActivableNode.h"

namespace phx {

ActivableNode::ActivableNode(bool isEnabled) noexcept
    : m_isEnabled{isEnabled} {}

void ActivableNode::accept(NodeVisitor &visitor) {
  if (visitor.visit(*this) && m_isEnabled) {
    processChildren(visitor);
  }
}

} // namespace phx
