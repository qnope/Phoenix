#include "SkyPass.h"

namespace phx {
SkySubpass::SkySubpass(GraphicPipeline pipeline) : m_pipeline{std::move(pipeline)} {}

void SkySubpass::setSkyInformations(const SkyInformations &skyInformations) noexcept {
    m_skyInformations = skyInformations;
}

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const SkySubpass &pass) {
    CommandBufferWrapper wrapper{cmdBuffer};

    const auto &layout = pass.m_pipeline.layout();

    wrapper.bindGraphicPipeline(pass.m_pipeline.getHandle());

    layout.push<SkyPushConstantRange>(wrapper, pass.m_skyInformations);

    cmdBuffer.draw(4, 1, 0, 0);
    return cmdBuffer;
}
} // namespace phx
