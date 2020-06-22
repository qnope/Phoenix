#include "DepthSubpass.h"

#include <ltl/Range/DefaultView.h>
#include <ltl/Range/enumerate.h>

namespace phx {

DepthSubpass::DepthSubpass(GraphicPipeline pipeline) : m_pipeline{std::move(pipeline)} {}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const DepthSubpass &pass) {
    assert(pass.m_drawBatches != nullptr);
    assert(pass.m_descriptorSet);

    const auto &layout = pass.m_pipeline.layout();

    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pass.m_pipeline.getHandle());
    layout.bind(cmdBuffer, vk::PipelineBindPoint::eGraphics, *pass.m_descriptorSet);

    for (auto [index, drawInformations] : ltl::enumerate(*pass.m_drawBatches | ltl::get(0_n))) {

        cmdBuffer.bindVertexBuffers(0, drawInformations.vertexBuffer.getHandle(), vk::DeviceSize(0));
        cmdBuffer.bindIndexBuffer(drawInformations.indexBuffer.getHandle(), 0, vk::IndexType::eUint32);

        layout.push<MatrixPushConstant>(cmdBuffer, uint32_t(index));
        cmdBuffer.drawIndexed(drawInformations.indexCount, 1, drawInformations.firstIndex,
                              drawInformations.vertexOffset, 0);
    }
    return cmdBuffer;
}

void DepthSubpass::setMatrixBufferAndDrawBatches(DescriptorSet matrixBufferDescriptorSet,
                                                 const std::vector<DrawBatche> *drawBatches) noexcept {
    m_drawBatches = drawBatches;
    m_descriptorSet = matrixBufferDescriptorSet;
}

} // namespace phx
