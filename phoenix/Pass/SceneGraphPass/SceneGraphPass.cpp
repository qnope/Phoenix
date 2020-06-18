#include "SceneGraphPass.h"

#include <SceneGraph/SceneGraph.h>
#include <vkw/Buffer/Buffer.h>
#include <vkw/Device.h>

namespace phx {

using MatrixBufferBinding =
    DescriptorBinding<VK_SHADER_STAGE_VERTEX_BIT,
                      vk::DescriptorType::eStorageBuffer, 1, glm::mat4>;

using Layout = DescriptorSetLayout<MatrixBufferBinding>;

class SceneGraphPass::Impl {
public:
  Impl(Device &device) noexcept
      : m_poolManager{device}, //
        m_buffer{
            device.createBuffer<CpuStorageBuffer<glm::mat4>>(10'000'000)}, //
        m_descriptorSet{m_poolManager.allocate<Layout>({{m_buffer}})}      //
  {}

  DescriptorSet getDescriptorSet() const noexcept { return m_descriptorSet; }

  void clearBuffer() noexcept { m_buffer.clear(); }

  void push_value(glm::mat4 matrix) noexcept { m_buffer << matrix; }

private:
  DescriptorPoolManager m_poolManager;
  CpuStorageBuffer<glm::mat4> m_buffer;
  DescriptorSet m_descriptorSet;
};

SceneGraphPass::SceneGraphPass(Device &device) noexcept
    : m_impl{std::make_unique<Impl>(device)} {}

ltl::tuple_t<DescriptorSet, std::vector<DrawBatche>>
SceneGraphPass::generate(SceneGraph &sceneGraph) noexcept {
  auto drawBatches = sceneGraph.dispatch(GetDrawBatchesVisitor{});

  m_impl->clearBuffer();

  for (glm::mat4 matrix : drawBatches | ltl::get(0_n)) {
    m_impl->push_value(matrix);
  }

  return {m_impl->getDescriptorSet(), drawBatches | ltl::get(1_n)};
}

SceneGraphPass::~SceneGraphPass() = default;

} // namespace phx
