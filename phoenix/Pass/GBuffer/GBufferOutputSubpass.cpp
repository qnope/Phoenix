#include "GBufferOutputSubpass.h"

#include <ltl/Range/enumerate.h>
#include <ltl/functional.h>

namespace phx {

void GBufferOutputSubpass::addGraphicPipeline(GraphicPipeline pipeline) { m_pipelines.push_back(std::move(pipeline)); }

GraphicPipeline GBufferOutputSubpass::getCompatiblePipeline(const Material &material) const noexcept {
    auto hasCompatible = [&material](const GraphicPipeline &pipeline) {
        return material.isCompatibleWith(pipeline.layout());
    };

    assert(ltl::contains_if(m_pipelines, hasCompatible));
    return *ltl::find_if_ptr(m_pipelines, hasCompatible);
}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const GBufferOutputSubpass &pass) noexcept {
    assert(pass.m_descriptorSet);

    CommandBufferWrapper wrapper{cmdBuffer};

    for (auto [drawInformationsAndMaterial, index] : pass.m_drawBatches) {
        const auto &[drawInformations, material] = drawInformationsAndMaterial;

        auto pipeline = pass.getCompatiblePipeline(material);
        const auto &pipelineLayout = pipeline.layout();
        wrapper.bindGraphicPipeline(pipeline.getHandle());
        pipelineLayout.bind(wrapper, vk::PipelineBindPoint::eGraphics, *pass.m_descriptorSet);

        wrapper.bindIndexBuffer(drawInformations.indexBuffer.getHandle());
        wrapper.bindVertexBuffer(drawInformations.vertexBuffer.getHandle());

        material.bindTo(wrapper, pipelineLayout);

        pipelineLayout.push<MatrixPushConstant>(wrapper, uint32_t(index));
        cmdBuffer.drawIndexed(drawInformations.indexCount, 1, drawInformations.firstIndex,
                              drawInformations.vertexOffset, 0);
    }
    return cmdBuffer;
}

void GBufferOutputSubpass::setMatrixBufferAndDrawBatches(
    DescriptorSet matrixBufferDescriptorSet, std::vector<ltl::tuple_t<DrawBatche, uint32_t>> drawBatches) noexcept {
    m_descriptorSet = matrixBufferDescriptorSet;
    m_drawBatches = std::move(drawBatches);

    auto comparator = [](const auto &drawBatchIndex1, const auto &drawBatchIndex2) {
        const auto &mtl1 = drawBatchIndex1[0_n][1_n];
        const auto &mtl2 = drawBatchIndex2[0_n][1_n];

        const auto &drawInfo1 = drawBatchIndex1[0_n][0_n];
        const auto &drawInfo2 = drawBatchIndex2[0_n][0_n];

        if (mtl1 == mtl2) {
            return drawInfo1 < drawInfo2;
        }
        return mtl1 < mtl2;
    };

    ltl::sort(m_drawBatches, comparator);
}

} // namespace phx
