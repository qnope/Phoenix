#include "SceneGraphPass.h"

#include <SceneGraph/SceneGraph.h>
#include <vkw/Buffer/Buffer.h>
#include <vkw/Device.h>

namespace phx {

class SceneGraphPass::Impl {
  public:
    Impl(Device &device) noexcept :
        m_poolManager{device},                                                    //
        m_buffer{device.createBuffer<CpuStorageBuffer<glm::mat4>>(10'000'000)},   //
        m_descriptorSet{m_poolManager.allocate<MatrixBufferLayout>({{m_buffer}})} //
    {}

    const auto &layout() noexcept { return m_poolManager.layout<MatrixBufferLayout>(); }

    DescriptorSet getDescriptorSet() const noexcept { return m_descriptorSet; }

    void clearBuffer() noexcept { m_buffer.clear(); }

    void push_value(glm::mat4 matrix) noexcept { m_buffer << matrix; }

  private:
    DescriptorPoolManager m_poolManager;
    CpuStorageBuffer<glm::mat4> m_buffer;
    DescriptorSet m_descriptorSet;
};

SceneGraphPass::SceneGraphPass(Device &device) noexcept : m_impl{std::make_unique<Impl>(device)} {}

ltl::tuple_t<DescriptorSet, std::vector<DrawBatche>> SceneGraphPass::generate(SceneGraph &sceneGraph) noexcept {
    auto drawBatches = sceneGraph.dispatch(GetDrawBatchesVisitor{});

    m_impl->clearBuffer();

    for (glm::mat4 matrix : drawBatches | ltl::get(0_n)) {
        m_impl->push_value(matrix);
    }

    return {m_impl->getDescriptorSet(), drawBatches | ltl::get(1_n)};
}

const MatrixBufferLayout &SceneGraphPass::matrixBufferLayout() const noexcept { return m_impl->layout(); }

SceneGraphPass::~SceneGraphPass() = default;

} // namespace phx
