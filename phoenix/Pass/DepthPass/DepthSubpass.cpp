#include "DepthSubpass.h"

#include <ltl/Range/DefaultView.h>
#include <ltl/Range/enumerate.h>
#include <ltl/operator.h>

#include <vkw/CommandBufferWrapper.h>

namespace phx {

DepthSubpass::DepthSubpass(GraphicPipeline pipeline) : m_pipeline{std::move(pipeline)} {}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const DepthSubpass &pass) {
    assert(pass.m_descriptorSet);

    CommandBufferWrapper wrapper{cmdBuffer};
    const auto &layout = pass.m_pipeline.layout();

    wrapper.bindGraphicPipeline(pass.m_pipeline.getHandle());
    layout.bind(wrapper, vk::PipelineBindPoint::eGraphics, *pass.m_descriptorSet);

    for (auto [drawInfo, index] : pass.m_drawBatches) {
        wrapper.bindIndexBuffer(drawInfo.indexBuffer.getHandle());
        wrapper.bindVertexBuffer(drawInfo.vertexBuffer.getHandle());
        layout.push<MatrixPushConstant>(wrapper, uint32_t(index));
        cmdBuffer.drawIndexed(drawInfo.indexCount, 1, drawInfo.firstIndex, drawInfo.vertexOffset, 0);
    }
    return cmdBuffer;
}

void DepthSubpass::setMatrixBufferAndDrawBatches(
    DescriptorSet matrixBufferDescriptorSet,
    std::vector<ltl::tuple_t<DrawInformations, uint32_t>> drawBatches) noexcept {
    m_drawBatches = std::move(drawBatches);
    m_descriptorSet = matrixBufferDescriptorSet;

    auto comparator = [](const auto &drawInfoAndIndex1, const auto &drawInfoAndIndex2) {
        const DrawInformations &drawInfo1 = drawInfoAndIndex1[0_n];
        const DrawInformations &drawInfo2 = drawInfoAndIndex2[0_n];
        return drawInfo1 < drawInfo2;
    };
    ltl::sort(m_drawBatches, comparator);
}

} // namespace phx
