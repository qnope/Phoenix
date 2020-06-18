#pragma once

#include <vkw/Device.h>
#include <vkw/GraphicPipeline.h>

#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>

#include "../SceneGraphPass/SceneGraphPass.h"

namespace phx {

class DepthSubpass : public phx::AbstractSubpass {
public:
  DepthSubpass(GraphicPipeline pipeline);

  friend vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                                      const DepthSubpass &pass);

  void setMatrixBufferAndDrawBatches(
      DescriptorSet matrixBufferDescriptorSet,
      const std::vector<DrawBatche> *drawBatches) noexcept;

private:
  GraphicPipeline m_pipeline;
  std::optional<DescriptorSet> m_descriptorSet;
  const std::vector<DrawBatche> *m_drawBatches = nullptr;
};

template <typename RenderPass>
auto make_depth_subpass(Device &device, Width width, Height height,
                        const RenderPass &renderPass,
                        const MatrixBufferLayout &layout) {
  auto vertexShader = device.createShaderModule<VertexShaderType>(
      "../phoenix/shaders/DepthPass/DepthPass.vert", true);
  auto fragmentShader = device.createShaderModule<FragmentShaderType>(
      "../phoenix/shaders/DepthPass/DepthPass.frag", true);
  auto pipelineLayout =
      device.createPipelineLayout(ltl::tuple_t{MatrixPushConstant{}}, //
                                  ltl::tuple_t{std::cref(layout)});

  auto vertexBinding = BindingDescription{
      0_n, ltl::type_v<Complete3dVertex>, Complete3dVertex::stride,
      vk::VertexInputAttributeDescription(0, 0, Position3D::format, 0)};

  vk::PipelineDepthStencilStateCreateInfo depthInfo{};
  depthInfo.depthTestEnable = true;
  depthInfo.depthWriteEnable = true;
  depthInfo.depthCompareOp = vk::CompareOp::eLess;

  return DepthSubpass{device.createGraphicPipeline(
      std::move(pipelineLayout), renderPass, 0_n,
      WithBindingDescriptions{vertexBinding},
      WithShaders{std::move(vertexShader), std::move(fragmentShader)},
      WithViewports{viewport::StaticViewport{width, height}},
      WithScissors{scissor::StaticScissor{width, height}}, //
      depthInfo)};
}

} // namespace phx
