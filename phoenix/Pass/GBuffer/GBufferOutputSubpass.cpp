#include "GBufferOutputSubpass.h"

#include <ltl/functional.h>

namespace phx {

void GBufferOutputSubpass::addGraphicPipeline(GraphicPipeline pipeline) {
  m_pipelines.push_back(std::move(pipeline));
}

GraphicPipeline
GBufferOutputSubpass::getCompatiblePipeline(const Material &material) const
    noexcept {
  auto hasCompatible = [&material](const GraphicPipeline &pipeline) {
    return material.isCompatibleWith(pipeline.layout());
  };

  assert(ltl::contains_if(m_pipelines, hasCompatible));
  return *ltl::find_if_ptr(m_pipelines, hasCompatible);
}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                             const GBufferOutputSubpass &pass) noexcept {
  assert(pass.m_drawBatches != nullptr);

  for (auto [drawInformations, material] : *pass.m_drawBatches) {
    auto pipeline = pass.getCompatiblePipeline(material);
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                           pipeline.getHandle());

    cmdBuffer.bindVertexBuffers(0, drawInformations.vertexBuffer.getHandle(),
                                vk::DeviceSize(0));
    cmdBuffer.bindIndexBuffer(drawInformations.indexBuffer.getHandle(), 0,
                              vk::IndexType::eUint32);

    material.bindTo(cmdBuffer, pipeline.layout());

    cmdBuffer.drawIndexed(drawInformations.indexCount, 1,
                          drawInformations.firstIndex,
                          drawInformations.vertexOffset, 0);
  }
  return cmdBuffer;
}

void GBufferOutputSubpass::setMatrixBufferAndDrawBatches(
    DescriptorSet matrixBufferDescriptorSet,
    const std::vector<DrawBatche> *drawBatches) noexcept {
  m_descriptorSet = matrixBufferDescriptorSet;
  m_drawBatches = drawBatches;
}

} // namespace phx
