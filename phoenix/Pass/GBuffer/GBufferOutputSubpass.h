#pragma once

#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/Device.h>

#include <SceneGraph/Materials/TexturedLambertianMaterial.h>
#include <SceneGraph/SceneGraph.h>
#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>

#include <vkw/CommandBufferWrapper.h>

namespace phx {

class GBufferOutputSubpass : public phx::AbstractSubpass {
public:
  void addGraphicPipeline(GraphicPipeline pipeline);

  friend vk::CommandBuffer
  operator<<(vk::CommandBuffer cmdBuffer,
             const GBufferOutputSubpass &pass) noexcept;

  void setDrawBatches(const std::vector<DrawBatche> *drawBatches) noexcept;

  GraphicPipeline getCompatiblePipeline(const Material &material) const
      noexcept;

private:
  std::vector<GraphicPipeline> m_pipelines;
  const std::vector<DrawBatche> *m_drawBatches = nullptr;
};

template <typename RenderPass>
auto make_gbuffer_output_subpass(Device &device, Width width, Height height,
                                 DescriptorPoolManager &poolManager,
                                 const RenderPass &renderPass) {
  auto vertexShader = device.createShaderModule<VertexShaderType>(
      "../phoenix/shaders/GBufferPass/GBufferOutput.vert", true);
  auto fragmentShader = device.createShaderModule<FragmentShaderType>(
      "../phoenix/shaders/GBufferPass/GBufferOutput.frag", true);

  const auto &layout =
      poolManager.layout<TexturedLambertianMaterialSetLayout>();
  auto pipelineLayout = device.createPipelineLayout(layout);

  auto vertexBinding = phx::Complete3dVertex::getBindingDescription(0_n);

  auto graphicPipeline = device.createGraphicPipeline(
      std::move(pipelineLayout), renderPass, 0_n,
      WithBindingDescriptions{vertexBinding},
      WithShaders{std::move(vertexShader), std::move(fragmentShader)},
      WithViewports{viewport::StaticViewport{width, height}},
      WithScissors{scissor::StaticScissor{width, height}},
      WithOutputs{output::normal_attachment});

  GBufferOutputSubpass outputSubpass;
  outputSubpass.addGraphicPipeline(std::move(graphicPipeline));

  return outputSubpass;
}

} // namespace phx
