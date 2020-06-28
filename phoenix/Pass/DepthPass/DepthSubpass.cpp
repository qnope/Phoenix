#include "DepthSubpass.h"

#include <ltl/Range/DefaultView.h>
#include <ltl/Range/enumerate.h>

namespace phx {

DepthSubpass::DepthSubpass(GraphicPipeline pipeline) : m_pipeline{std::move(pipeline)} {}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const DepthSubpass &pass) {
    assert(pass.m_descriptorSet);

    const auto &layout = pass.m_pipeline.layout();

    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pass.m_pipeline.getHandle());
    layout.bind(cmdBuffer, vk::PipelineBindPoint::eGraphics, *pass.m_descriptorSet);

    for (auto [drawInformations, index] : pass.m_drawBatches) {
        cmdBuffer.bindVertexBuffers(0, drawInformations.vertexBuffer.getHandle(), vk::DeviceSize(0));
        cmdBuffer.bindIndexBuffer(drawInformations.indexBuffer.getHandle(), 0, vk::IndexType::eUint32);

        layout.push<MatrixPushConstant>(cmdBuffer, uint32_t(index));
        cmdBuffer.drawIndexed(drawInformations.indexCount, 1, drawInformations.firstIndex,
                              drawInformations.vertexOffset, 0);
    }
    return cmdBuffer;
}

void DepthSubpass::setMatrixBufferAndDrawBatches(
    DescriptorSet matrixBufferDescriptorSet,
    std::vector<ltl::tuple_t<DrawInformations, uint32_t>> drawBatches) noexcept {
    m_drawBatches = std::move(drawBatches);
    m_descriptorSet = matrixBufferDescriptorSet;
}

} // namespace phx
