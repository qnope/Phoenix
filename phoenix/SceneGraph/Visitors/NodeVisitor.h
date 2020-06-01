#pragma once

#include "../Nodes/AbstractNode.h"

namespace phx {
class NodeVisitor {
public:
  virtual bool visit(AbstractNode &node) = 0;

protected:
  virtual ~NodeVisitor() = default;
};
} // namespace phx
