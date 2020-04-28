#pragma once
#include "../Phoenix/vkw/Buffer/Buffer.h"
#include "../phoenix/constant.h"
#include "../phoenix/vkw/Device.h"
#include "../phoenix/vkw/GraphicPipeline.h"
#include "../phoenix/vkw/Vertex.h"
#include <ltl/ltl.h>

template <typename Pipeline>
class TriangleSubpass : public phx::AbstractSubpass {
  typed_static_assert_msg(
      phx::is_graphic_pipeline(ltl::type_v<Pipeline>),
      "The template paremeter Pipeline must be GraphicPipeline");

public:
  TriangleSubpass(Pipeline pipeline,
                  phx::CpuVertexBuffer<phx::Colored2DVertex> &buffer)
      : m_pipeline{std::move(pipeline)}, m_buffer{buffer} {}

  friend auto operator<<(vk::CommandBuffer cmdBuffer,
                         const TriangleSubpass &subpass) noexcept {
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                           subpass.m_pipeline.getHandle());
    cmdBuffer.bindVertexBuffers(0, subpass.m_buffer.getHandle(),
                                vk::DeviceSize(0));
    cmdBuffer.draw(3, 1, 0, 0);
    return cmdBuffer;
  }

private:
  Pipeline m_pipeline;
  phx::CpuVertexBuffer<phx::Colored2DVertex> &m_buffer;
};

template <typename... RP>
auto make_triangle_pass(phx::Device &device, phx::Width width,
                        phx::Height height,
                        const phx::RenderPass<RP...> &renderPass,
                        phx::CpuVertexBuffer<phx::Colored2DVertex> &buffer) {
  auto vertexShader = device.createShaderModule<phx::VertexShaderType>(
      "../phoenix/shaders/TriangleTest/triangle.vert", true);

  auto fragmentShader = device.createShaderModule<phx::FragmentShaderType>(
      "../phoenix/shaders/TriangleTest/triangle.frag", true);

  auto pipelineLayout = device.createPipelineLayout();

  auto binding = phx::Colored2DVertex::getBindingDescription(0_n);

  auto graphicPipeline = device.createGraphicPipeline(
      std::move(pipelineLayout), renderPass, 0_n,
      phx::WithBindingDescriptions{binding},
      phx::WithShaders{std::move(vertexShader), std::move(fragmentShader)},
      vk::PrimitiveTopology::eTriangleList,
      phx::WithViewports{phx::viewport::StaticViewport{width, height}},
      phx::WithScissors{phx::scissor::StaticScissor{width, height}},
      vk::CullModeFlagBits::eNone, vk::PolygonMode::eFill,
      phx::WithOutputs{phx::output::normal_attachment});

  return TriangleSubpass{std::move(graphicPipeline), buffer};
}
