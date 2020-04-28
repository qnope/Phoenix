#pragma once
#include "../Phoenix/vkw/Buffer/BufferRef.h"
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
                  phx::VertexBufferRef<phx::Colored2DVertex> vertexBuffer,
                  phx::IndexBufferRef<uint32_t> indexBuffer)
      : m_pipeline{std::move(pipeline)}, //
        m_vertexBuffer{vertexBuffer},    //
        m_indexBuffer{indexBuffer} {}

  friend auto operator<<(vk::CommandBuffer cmdBuffer,
                         const TriangleSubpass &subpass) noexcept {
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                           subpass.m_pipeline.getHandle());
    cmdBuffer.bindVertexBuffers(0, subpass.m_vertexBuffer.getHandle(),
                                vk::DeviceSize(0));
    cmdBuffer.bindIndexBuffer(subpass.m_indexBuffer.getHandle(), 0,
                              vk::IndexType::eUint32);
    cmdBuffer.drawIndexed(6, 1, 0, 0, 0);
    return cmdBuffer;
  }

private:
  Pipeline m_pipeline;
  phx::VertexBufferRef<phx::Colored2DVertex> m_vertexBuffer;
  phx::IndexBufferRef<uint32_t> m_indexBuffer;
};

template <typename... RP>
auto make_triangle_pass(phx::Device &device, phx::Width width,
                        phx::Height height,
                        const phx::RenderPass<RP...> &renderPass,
                        phx::VertexBufferRef<phx::Colored2DVertex> vertexBuffer,
                        phx::IndexBufferRef<uint32_t> indexBuffer) {
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

  return TriangleSubpass{std::move(graphicPipeline), vertexBuffer, indexBuffer};
}
