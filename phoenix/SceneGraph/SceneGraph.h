#pragma once

#include <vkw/Descriptor/DescriptorPoolManager.h>

#include "DrawInformations/DrawInformationsAllocator.h"
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

private:
  void prepareDescriptorPools(Device &device);

private:
  Node m_rootNode;
  DrawInformationsAllocator m_drawInfoAllocator;
  DescriptorPoolManager m_descriptorPoolManager;
};

} // namespace phx
