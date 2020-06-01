#include "SceneGraph.h"

#include "Nodes/GroupNode.h"

namespace phx {

SceneGraph::SceneGraph(Device &device) noexcept
    : m_rootNode{GroupNode{}}, //
      m_drawInfoAllocator{device} {}

SceneGraph::SceneGraph(Device &device, Node rootNode) noexcept
    : m_rootNode{std::move(rootNode)}, //
      m_drawInfoAllocator{device} {}

void SceneGraph::setRootNode(Node rootNode) noexcept {
  m_rootNode = std::move(rootNode);
}

} // namespace phx
