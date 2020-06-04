#include <vkw/Device.h>

#include "Nodes/GroupNode.h"
#include "SceneGraph.h"

#include "Materials/TexturedLambertianMaterial.h"

namespace phx {

SceneGraph::SceneGraph(Device &device) noexcept
    : m_rootNode{GroupNode{}}, //
      m_drawInfoAllocator{device} {
  prepareDescriptorPools(device);
}

SceneGraph::SceneGraph(Device &device, Node rootNode) noexcept
    : m_rootNode{std::move(rootNode)}, //
      m_drawInfoAllocator{device} {}

void SceneGraph::setRootNode(Node rootNode) noexcept {
  m_rootNode = std::move(rootNode);
}

void SceneGraph::prepareDescriptorPools(Device &device) {
  m_descriptorPoolManager.addDescriptorPool(
      device.createDescriptorPool<TexturedLambertianMaterialSetLayout>());
}

} // namespace phx
