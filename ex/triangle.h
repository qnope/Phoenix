#pragma once
#include "../phoenix/constant.h"
#include "../phoenix/vkw/CommandBufferWrapper.h"

#include "../phoenix/vkw/Buffer/BufferInfo.h"
#include "../phoenix/vkw/Device.h"
#include "../phoenix/vkw/GraphicPipeline.h"
#include "../phoenix/vkw/Vertex.h"
#include <ltl/ltl.h>

template <typename Pipeline, typename DescriptorSet>
class TriangleSubpass : public phx::AbstractSubpass {
  typed_static_assert_msg(
      phx::is_graphic_pipeline(ltl::type_v<Pipeline>),
      "The template paremeter Pipeline must be GraphicPipeline");

public:
  TriangleSubpass(Pipeline pipeline, phx::VertexBufferInfo vertexBuffer,
                  phx::IndexBufferInfo indexBuffer, DescriptorSet set)
      : m_pipeline{std::move(pipeline)}, //
        m_vertexBuffer{vertexBuffer},    //
        m_indexBuffer{indexBuffer},      //
        m_set{set} {}

  friend auto operator<<(vk::CommandBuffer cmdBuffer,
                         const TriangleSubpass &subpass) noexcept {
    phx::CommandBufferWrapper commandBufferWrapper{cmdBuffer};

    commandBufferWrapper.bindVertexBuffersToGraphicPipeline(
        subpass.m_pipeline, subpass.m_vertexBuffer.buffer);

    commandBufferWrapper.bindIndexBufferToGraphicPipeline(
        subpass.m_pipeline, subpass.m_indexBuffer.buffer);

    const auto &pipelineLayout = subpass.m_pipeline.pipelineLayout();

    commandBufferWrapper.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                            pipelineLayout, subpass.m_set);

    cmdBuffer.drawIndexed(subpass.m_indexBuffer.size, 1, 0, 0, 0);
    return cmdBuffer;
  }

private:
  Pipeline m_pipeline;
  phx::VertexBufferInfo m_vertexBuffer;
  phx::IndexBufferInfo m_indexBuffer;
  DescriptorSet m_set;
};

template <typename... RP, typename DescriptorPool, typename DescriptorSet>
auto make_triangle_pass(phx::Device &device, phx::Width width,
                        phx::Height height,
                        const phx::RenderPass<RP...> &renderPass,
                        phx::VertexBufferInfo vertexBuffer,
                        phx::IndexBufferInfo indexBuffer,
                        const DescriptorPool &pool, DescriptorSet set) {
  auto vertexShader = device.createShaderModule<phx::VertexShaderType>(
      "../phoenix/shaders/TriangleTest/triangle.vert", true);

  auto fragmentShader = device.createShaderModule<phx::FragmentShaderType>(
      "../phoenix/shaders/TriangleTest/triangle.frag", true);

  auto pipelineLayout = device.createPipelineLayout(pool.layout());

  auto vertexBinding = phx::Textured2dVertex::getBindingDescription(0_n);

  auto graphicPipeline = device.createGraphicPipeline(
      std::move(pipelineLayout), renderPass, 0_n,
      phx::WithBindingDescriptions{vertexBinding},
      phx::WithShaders{std::move(vertexShader), std::move(fragmentShader)},
      phx::WithViewports{phx::viewport::StaticViewport{width, height}},
      phx::WithScissors{phx::scissor::StaticScissor{width, height}},
      phx::WithOutputs{phx::output::normal_attachment});

  return TriangleSubpass{std::move(graphicPipeline), vertexBuffer, indexBuffer,
                         set};
}
