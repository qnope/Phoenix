#include "SceneGraphPass.h"

#include <SceneGraph/SceneGraph.h>
#include <ltl/Range/DefaultView.h>
#include <vkw/Buffer/Buffer.h>
#include <vkw/Descriptor/DescriptorPoolManager.h>
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
        auto proj = glm::perspective(glm::radians(45.f), 1024.0f / 768.0f, 1.0f, 10000.f);
        proj[1][1] *= -1;
        m_impl->push_value(proj *
                           glm::lookAt(glm::vec3{700.0f, 700.0f, 000.0f}, glm::vec3{699.0f, 700.0f, 0.0f},
                                       glm::vec3{0.0f, 1.0f, 0.0f}) *
                           matrix);
    }

    return {m_impl->getDescriptorSet(), drawBatches | ltl::get(1_n)};
}

const MatrixBufferLayout &SceneGraphPass::matrixBufferLayout() const noexcept { return m_impl->layout(); }

SceneGraphPass::~SceneGraphPass() = default;

} // namespace phx
