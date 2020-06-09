#include "PresentationSubpass.h"

namespace phx {

PresentationSubpass::PresentationSubpass(GraphicPipeline pipeline,
                                         DescriptorSet set) noexcept
    : m_pipeline{std::move(pipeline)}, //
      m_descriptorSet{set} {}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                             const PresentationSubpass &pass) noexcept {

  cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                         pass.m_pipeline.getHandle());
  pass.m_pipeline.layout().bind(cmdBuffer, vk::PipelineBindPoint::eGraphics, 0,
                                pass.m_descriptorSet);
  cmdBuffer.draw(4, 1, 0, 0);
  return cmdBuffer;
}

} // namespace phx
