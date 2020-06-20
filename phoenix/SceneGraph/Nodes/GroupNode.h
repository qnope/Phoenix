#pragma once

#include <vector>

#include "../Visitors/NodeVisitor.h"
#include "AbstractNode.h"
#include "Node.h"

namespace phx {

class BaseGroupNode : public AbstractNode {
  public:
    void addChild(Node node);

    void processChildren(NodeVisitor &visitor);

  protected:
    std::vector<Node> m_nodes;
};

class GroupNode : public BaseGroupNode {
  public:
    void accept(NodeVisitor &visitor);
};

} // namespace phx
