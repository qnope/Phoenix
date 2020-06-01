#pragma once

#include "Nodes/Node.h"

namespace phx {

class SceneGraph {
public:
  SceneGraph() noexcept;
  SceneGraph(Node rootNode) noexcept;

  void setRootNode(Node rootNode) noexcept;

  template <typename ConcreteNode> ConcreteNode *rootNode() {
    return m_rootNode.getConcretePtr<ConcreteNode>();
  }

  template <typename Visitor> auto dispatch(Visitor &&visitor) {
    return m_rootNode.accept(visitor);
  }

private:
  Node m_rootNode;
};

} // namespace phx
