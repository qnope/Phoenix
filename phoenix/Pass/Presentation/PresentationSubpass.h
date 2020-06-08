#pragma once

#include <vkw/Device.h>
#include <vkw/GraphicPipeline.h>

namespace phx {

class PresentationSubpass : AbstractSubpass {
public:
  PresentationSubpass(GraphicPipeline pipeline, vk::DescriptorSet set) noexcept;

  friend vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                                      const PresentationSubpass &pass) noexcept;

private:
  GraphicPipeline m_pipeline;
  vk::DescriptorSet m_descriptorSet;
};

template <typename RenderPass, typename Layout>
auto make_presentation_pipeline(Device &device, const RenderPass &renderPass,
                                const Layout &layout, Width width,
                                Height height) {
  auto vertexShader = device.createShaderModule<VertexShaderType>(
      "../phoenix/shaders/PresentationPass/PresentationPass.vert", true);
  auto fragmentShader = device.createShaderModule<FragmentShaderType>(
      "../phoenix/shaders/PresentationPass/PresentationPass.frag", true);

  auto pipelineLayout = device.createPipelineLayout(layout);

  return device.createGraphicPipeline(
      std::move(pipelineLayout), renderPass, 0_n,
      WithShaders{std::move(vertexShader), std::move(fragmentShader)},
      WithViewports{viewport::StaticViewport{width, height}},
      WithScissors{scissor::StaticScissor{width, height}},
      WithOutputs{output::normal_attachment},
      vk::PrimitiveTopology::eTriangleStrip);
}

} // namespace phx
