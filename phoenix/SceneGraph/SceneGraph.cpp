#include "SceneGraph.h"

#include "Nodes/GroupNode.h"

namespace phx {

SceneGraph::SceneGraph() noexcept : m_rootNode{GroupNode{}} {}

SceneGraph::SceneGraph(Node rootNode) noexcept
    : m_rootNode{std::move(rootNode)} {}

void SceneGraph::setRootNode(Node rootNode) noexcept {
  m_rootNode = std::move(rootNode);
}

} // namespace phx
