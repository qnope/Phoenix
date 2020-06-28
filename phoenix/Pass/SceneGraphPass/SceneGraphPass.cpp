#include "SceneGraphPass.h"

#include <SceneGraph/SceneGraph.h>
#include <ltl/Range/DefaultView.h>
#include <vkw/Buffer/Buffer.h>
#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/Device.h>

#include <ltl/Range/enumerate.h>

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

    void setLookAtMatrix(glm::mat4 matrix) noexcept { m_buffer.ptr()[1] = matrix; }

    void setProjectionMatrix(glm::mat4 matrix) noexcept { m_buffer.ptr()[0] = matrix; }

  private:
    DescriptorPoolManager m_poolManager;
    CpuStorageBuffer<glm::mat4> m_buffer;
    DescriptorSet m_descriptorSet;
};

SceneGraphPass::SceneGraphPass(Device &device) noexcept : m_impl{std::make_unique<Impl>(device)} {}

void SceneGraphPass::setLookAtMatrix(glm::mat4 matrix) noexcept { m_impl->setLookAtMatrix(matrix); }

void SceneGraphPass::setProjectionMatrix(glm::mat4 matrix) noexcept { m_impl->setProjectionMatrix(matrix); }

ltl::tuple_t<DescriptorSet, std::vector<ltl::tuple_t<DrawBatche, uint32_t>>>
SceneGraphPass::generate(SceneGraph &sceneGraph) noexcept {
    auto matrixAndDrawBatches = sceneGraph.dispatch(GetDrawBatchesVisitor{});

    m_impl->clearBuffer();
    m_impl->push_value(glm::mat4{});
    m_impl->push_value(glm::mat4{});

    for (glm::mat4 matrix : matrixAndDrawBatches | ltl::get(0_n)) {
        m_impl->push_value(matrix);
    }

    auto drawBatches = ltl::enumerate(matrixAndDrawBatches | ltl::get(1_n));

    auto invert = ltl::unzip([](auto index, DrawBatche batch) { //
        return ltl::tuple_t{batch, uint32_t(index)};
    });

    return {m_impl->getDescriptorSet(), drawBatches | ltl::map(invert)};
}

const MatrixBufferLayout &SceneGraphPass::matrixBufferLayout() const noexcept { return m_impl->layout(); }

SceneGraphPass::~SceneGraphPass() = default;

} // namespace phx
