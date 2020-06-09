#include "GBufferOutputSubpass.h"

namespace phx {

void GBufferOutputSubpass::addGraphicPipeline(GraphicPipeline pipeline) {
  m_pipelines.push_back(std::move(pipeline));
}

GraphicPipeline
GBufferOutputSubpass::getCompatiblePipeline(const Material &material) const
    noexcept {
  auto hasCompatible =
      [type = material.layoutType()](const GraphicPipeline &pipeline) {
        return pipeline.layout().isCompatible(type);
      };

  assert(ltl::contains_if(m_pipelines, hasCompatible));
  return *ltl::find_if_ptr(m_pipelines, hasCompatible);
}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                             const GBufferOutputSubpass &pass) noexcept {
  assert(pass.m_drawBatches != nullptr);

  for (auto [matrix, drawInformations, material] : *pass.m_drawBatches) {
    auto pipeline = pass.getCompatiblePipeline(material);
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                           pipeline.getHandle());

    cmdBuffer.bindVertexBuffers(0, drawInformations.vertexBuffer.getHandle(),
                                vk::DeviceSize(0));
    cmdBuffer.bindIndexBuffer(drawInformations.indexBuffer.getHandle(), 0,
                              vk::IndexType::eUint32);

    pipeline.layout().bind(cmdBuffer, vk::PipelineBindPoint::eGraphics, 0,
                           material.descriptorSet());

    cmdBuffer.drawIndexed(drawInformations.indexCount, 1,
                          drawInformations.firstIndex,
                          drawInformations.vertexOffset, 0);
  }
  return cmdBuffer;
}

void GBufferOutputSubpass::setDrawBatches(
    const std::vector<DrawBatche> *drawBatches) noexcept {
  m_drawBatches = drawBatches;
}

} // namespace phx
