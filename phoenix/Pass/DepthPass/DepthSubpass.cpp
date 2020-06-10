#include "DepthSubpass.h"

namespace phx {

DepthSubpass::DepthSubpass(GraphicPipeline pipeline)
    : m_pipeline{std::move(pipeline)} {}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                             const DepthSubpass &pass) {
  assert(pass.m_drawBatches != nullptr);

  cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                         pass.m_pipeline.getHandle());

  for (auto [matrix, drawInformations, material] : *pass.m_drawBatches) {
    cmdBuffer.bindVertexBuffers(0, drawInformations.vertexBuffer.getHandle(),
                                vk::DeviceSize(0));
    cmdBuffer.bindIndexBuffer(drawInformations.indexBuffer.getHandle(), 0,
                              vk::IndexType::eUint32);

    cmdBuffer.drawIndexed(drawInformations.indexCount, 1,
                          drawInformations.firstIndex,
                          drawInformations.vertexOffset, 0);

    (void)material;
  }
  return cmdBuffer;
}

void DepthSubpass::setDrawBatches(
    const std::vector<DrawBatche> *drawBatches) noexcept {
  m_drawBatches = drawBatches;
}

} // namespace phx
