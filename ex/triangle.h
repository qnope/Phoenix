#pragma once
#include "../phoenix/constant.h"
#include "../phoenix/vkw/Device.h"
#include "../phoenix/vkw/GraphicPipeline.h"
#include <ltl/ltl.h>

template <typename Pipeline> class TriangleSubpass {
  typed_static_assert_msg(phx::isGraphicPipeline(ltl::type_v<Pipeline>),
                          "The template paremeter Pipeline must be GraphicPipeline");

public:
  TriangleSubpass(Pipeline pipeline) : m_pipeline{std::move(pipeline)} {}

  auto operator()(vk::CommandBuffer cmdBuffer) const noexcept {
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.getHandle());
    cmdBuffer.draw(3, 1, 0, 0);
  }

private:
  Pipeline m_pipeline;
};

template <typename... RP>
auto make_triangle_pass(phx::Device &device, phx::Width width, phx::Height height,
                        const phx::RenderPass<RP...> &renderPass) {
  auto vertexShader = device.createShaderModule<phx::VertexShaderType>(
      "../Phoenix/phoenix/shaders/TriangleTest/triangle.vert", true);

  auto fragmentShader = device.createShaderModule<phx::FragmentShaderType>(
      "../Phoenix/phoenix/shaders/TriangleTest/triangle.frag", true);

  auto pipelineLayout = device.createPipelineLayout();

  auto graphicPipeline = device.createGraphicPipeline(
      std::move(pipelineLayout), renderPass, 0_n,
      phx::WithShaders{std::move(vertexShader), std::move(fragmentShader)},
      vk::PrimitiveTopology::eTriangleList,
      phx::WithViewports{phx::viewport::StaticViewport{width, height}},
      phx::WithScissors{phx::scissor::StaticScissor{width, height}},
      vk::CullModeFlagBits::eNone, vk::PolygonMode::eFill,
      phx::WithOutputs{phx::output::normal_attachment});

  return TriangleSubpass{std::move(graphicPipeline)};
}
