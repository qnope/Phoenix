#pragma once

#include "GroupNode.h"

namespace phx {

class ActivableNode : public BaseGroupNode {
  public:
    ActivableNode(bool isEnabled) noexcept;

    void accept(NodeVisitor &visitor);

  private:
    bool m_isEnabled = false;
};

} // namespace phx
