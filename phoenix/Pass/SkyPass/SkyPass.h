#pragma once

#include <vkw/Device.h>
#include <vkw/GraphicPipeline.h>
#include "SkyInformation.h"

namespace phx {

class SkySubpass : public phx::AbstractSubpass {
  public:
    SkySubpass(GraphicPipeline pipeline);

    void setSkyInformations(const SkyInformations &skyInformations) noexcept;

    friend vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const SkySubpass &pass);

  private:
    GraphicPipeline m_pipeline;
    SkyInformations m_skyInformations;
};

template <typename RenderPass>
auto make_sky_subpass(Device &device, Width width, Height height, const RenderPass &renderPass) {
    auto vertexShader = device.createShaderModule<VertexShaderType>("../phoenix/shaders/SkyPass/SkyPass.vert", true);
    auto fragmentShader =
        device.createShaderModule<FragmentShaderType>("../phoenix/shaders/SkyPass/SkyPass.frag", true);
    auto pipelineLayout = device.createPipelineLayout(phx::with_push_constants, SkyPushConstantRange{});

    vk::PipelineDepthStencilStateCreateInfo depthInfo{};
    depthInfo.depthTestEnable = true;
    depthInfo.depthWriteEnable = false;
    depthInfo.depthCompareOp = vk::CompareOp::eEqual;

    return SkySubpass{device.createGraphicPipeline(
        std::move(pipelineLayout), renderPass, 2_n, //
        WithShaders{std::move(vertexShader), std::move(fragmentShader)},
        WithViewports{viewport::StaticViewport{width, height}}, WithScissors{scissor::StaticScissor{width, height}}, //
        WithOutputs{output::normal_attachment}, vk::PrimitiveTopology::eTriangleStrip, depthInfo)};
}

} // namespace phx
