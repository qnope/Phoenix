#pragma once

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

  DrawInformations
  allocateDrawInformations(const std::vector<Complete3dVertex> &vertex,
                           const std::vector<uint32_t> &indices);

  void flush(vk::PipelineStageFlags nextPipeline,
             vk::AccessFlags nextAccess) noexcept;

private:
  Node m_rootNode;
  DrawInformationsAllocator m_drawInfoAllocator;
};

} // namespace phx
