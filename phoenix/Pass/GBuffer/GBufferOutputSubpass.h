#pragma once

#include <ltl/functional.h>

#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/Device.h>
#include <vkw/GraphicPipeline.h>

#include <SceneGraph/Materials/TexturedLambertianMaterial.h>
#include <SceneGraph/SceneGraph.h>
#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>

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

template <typename RenderPass, typename Layout>
auto make_gbuffer_output_pipeline(Device &device, Width width, Height height,
                                  DescriptorPoolManager &poolManager,
                                  const RenderPass &renderPass,
                                  ltl::type_t<Layout>,
                                  const std::string &fragmentPath) {
  auto vertexShader = device.createShaderModule<VertexShaderType>(
      "../phoenix/shaders/GBufferPass/GBufferOutput.vert", true);

  auto fragmentShader =
      device.createShaderModule<FragmentShaderType>(fragmentPath, true);

  const auto &layout = poolManager.layout<Layout>();
  auto pipelineLayout = device.createPipelineLayout(layout);

  auto vertexBinding = phx::Complete3dVertex::getBindingDescription(0_n);

  vk::PipelineDepthStencilStateCreateInfo depthInfo{};
  depthInfo.depthTestEnable = true;
  depthInfo.depthWriteEnable = false;
  depthInfo.depthCompareOp = vk::CompareOp::eEqual;

  return device.createGraphicPipeline(
      std::move(pipelineLayout), renderPass.get(), 1_n,
      WithBindingDescriptions{vertexBinding},
      WithShaders{std::move(vertexShader), std::move(fragmentShader)},
      WithViewports{viewport::StaticViewport{width, height}},
      WithScissors{scissor::StaticScissor{width, height}},
      WithOutputs{output::normal_attachment}, depthInfo);
}

template <typename RenderPass>
auto make_gbuffer_output_subpass(Device &device, Width width, Height height,
                                 DescriptorPoolManager &poolManager,
                                 const RenderPass &renderPass) {

  auto curried =
      ltl::curry(lift(make_gbuffer_output_pipeline), std::ref(device), width,
                 height, std::ref(poolManager), std::cref(renderPass));

  GBufferOutputSubpass outputSubpass;
  outputSubpass.addGraphicPipeline(GraphicPipeline{
      curried(ltl::type_v<TexturedLambertianMaterialSetLayout>,
              "../phoenix/shaders/GBufferPass/GBufferLambertianTexture.frag")});

  return outputSubpass;
}

} // namespace phx
