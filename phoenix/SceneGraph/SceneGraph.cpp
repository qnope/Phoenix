#include "SceneGraph.h"
#include "Nodes/GroupNode.h"

#include "Materials/TexturedLambertianMaterial.h"

namespace phx {

SceneGraph::SceneGraph(Device &device) noexcept :
    m_rootNode{GroupNode{}},    //
    m_drawInfoAllocator{device} //
{}

SceneGraph::SceneGraph(Device &device, Node rootNode) noexcept :
    m_rootNode{std::move(rootNode)}, //
    m_drawInfoAllocator{device}      //
{}

void SceneGraph::setRootNode(Node rootNode) noexcept { m_rootNode = std::move(rootNode); }

DrawInformations SceneGraph::allocateDrawInformations(const std::vector<Complete3dVertex> &vertex,
                                                      const std::vector<uint32_t> &indices) {
    return m_drawInfoAllocator.allocate(vertex, indices);
}

void SceneGraph::flush(vk::PipelineStageFlags nextPipeline, vk::AccessFlags nextAccess) noexcept {
    m_drawInfoAllocator.flush(nextPipeline, nextAccess);
}

} // namespace phx
