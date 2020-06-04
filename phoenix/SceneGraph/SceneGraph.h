#pragma once

#include "DrawInformations/DrawInformationsAllocator.h"
#include "Nodes/Node.h"

namespace phx {

class Device;

class SceneGraph {
public:
  SceneGraph(Device &device) noexcept;
  SceneGraph(Device &device, Node rootNode) noexcept;

  void setRootNode(Node rootNode) noexcept;

  template <typename ConcreteNode> ConcreteNode *rootNode() {
    return m_rootNode.get<ConcreteNode>();
  }

  template <typename Visitor> auto dispatch(Visitor &&visitor) {
    return m_rootNode.accept(visitor);
  }

private:
  Node m_rootNode;
  DrawInformationsAllocator m_drawInfoAllocator;
};

} // namespace phx
