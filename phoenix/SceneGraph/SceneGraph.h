#pragma once

#include "DrawInformations/DrawInformationsAllocator.h"
#include "Materials/MaterialFactory.h"
#include "Nodes/Node.h"

namespace phx {

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

  MaterialFactory &materialFactory() noexcept;
  DrawInformations allocate(const std::vector<Complete3dVertex> &vertex,
                            const std::vector<uint32_t> &indices);

private:
  Node m_rootNode;
  DrawInformationsAllocator m_drawInfoAllocator;
  MaterialFactory m_materialFactory;
};

} // namespace phx
