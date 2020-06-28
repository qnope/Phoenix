#include "PresentationSubpass.h"

namespace phx {

PresentationSubpass::PresentationSubpass(GraphicPipeline pipeline, DescriptorSet set) noexcept :
    m_pipeline{std::move(pipeline)}, //
    m_descriptorSet{set} {}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const PresentationSubpass &pass) noexcept {
    CommandBufferWrapper wrapper{cmdBuffer};
    wrapper.bindGraphicPipeline(pass.m_pipeline.getHandle());
    pass.m_pipeline.layout().bind(wrapper, vk::PipelineBindPoint::eGraphics, pass.m_descriptorSet);
    cmdBuffer.draw(4, 1, 0, 0);
    return cmdBuffer;
}

} // namespace phx
