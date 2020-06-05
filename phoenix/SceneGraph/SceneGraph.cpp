#include "SceneGraph.h"
#include "Nodes/GroupNode.h"

#include "Materials/TexturedLambertianMaterial.h"

namespace phx {

SceneGraph::SceneGraph(Device &device) noexcept
    : m_rootNode{GroupNode{}},     //
      m_drawInfoAllocator{device}, //
      m_materialFactory{device} {}

SceneGraph::SceneGraph(Device &device, Node rootNode) noexcept
    : m_rootNode{std::move(rootNode)}, //
      m_drawInfoAllocator{device},     //
      m_materialFactory{device} {}

void SceneGraph::setRootNode(Node rootNode) noexcept {
  m_rootNode = std::move(rootNode);
}

MaterialFactory &SceneGraph::materialFactory() noexcept {
  return m_materialFactory;
}

DrawInformations
SceneGraph::allocate(const std::vector<Complete3dVertex> &vertex,
                     const std::vector<uint32_t> &indices) {
  return m_drawInfoAllocator.allocate(vertex, indices);
}

} // namespace phx
