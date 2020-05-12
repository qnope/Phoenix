#pragma once

#include "Buffer/Buffer.h"
#include "GraphicPipeline.h"
#include <ltl/Tuple.h>
#include <ltl/condition.h>

constexpr std::size_t MAX_BINDING = 10;

namespace phx {
class CommandBufferWrapper {
public:
  CommandBufferWrapper(vk::CommandBuffer buffer) noexcept : m_buffer{buffer} {}

  template <typename Pipeline, typename... VertexBuffers>
  void bindVertexBuffersToGraphicPipeline(
      Pipeline &pipeline, VertexBuffers &... vertexBuffers) noexcept {
    typed_static_assert_msg(is_graphic_pipeline(pipeline),
                            "Pipeline must be a graphic pipeline");
    static_assert(sizeof...(VertexBuffers) < MAX_BINDING,
                  "Unable to have more than MAX_BINDING bindings");
    typed_static_assert_msg(
        (true_v && ... &&
         doesBufferSupport<vk::BufferUsageFlagBits::eVertexBuffer>(
             vertexBuffers)),
        "Buffers must be vertex buffers");

    auto pipelineVertexBufferTypes = pipeline.vertexBufferTypes;
    auto vertexBufferTypes = ltl::tuple_t{vertexBuffers.type...};
    typed_static_assert_msg(
        pipelineVertexBufferTypes.length == vertexBufferTypes.length,
        "Pipeline must be bound to the good number of Vertex Buffers");
    typed_static_assert_msg(pipelineVertexBufferTypes == vertexBufferTypes,
                            "Types of vertex buffer must match pipeline ones");

    auto vertexBuffersTuple =
        ltl::tuple_t<VertexBuffers &...>{vertexBuffers...};

    bindGraphicPipeline(pipeline);

    ltl::enumerate_with(
        [this](auto index, auto &vertexBuffer) {
          if (m_boundVertexBuffers[index.value] != vertexBuffer.getHandle()) {
            m_boundVertexBuffers[index.value] = vertexBuffer.getHandle();
            m_buffer.bindVertexBuffers(index.value, vertexBuffer.getHandle(),
                                       vk::DeviceSize{0});
          }
        },
        vertexBuffersTuple);
  }

  template <typename Pipeline, typename Buffer>
  void bindIndexBufferToGraphicPipeline(Pipeline &pipeline, Buffer &buffer) {
    typed_static_assert_msg(is_graphic_pipeline(pipeline),
                            "Pipeline must be a graphic pipeline");
    bindGraphicPipeline(pipeline);
    typed_static_assert_msg(
        (buffer.type ==
         ltl::AnyOf{ltl::type_v<uint16_t>, ltl::type_v<uint32_t>}),
        "Index Buffer must be an uint16_t or an uint32_t buffer");
    if (m_boundIndexBuffer != buffer.getHandle()) {
      m_boundIndexBuffer = buffer.getHandle();
      if_constexpr(buffer.type == ltl::type_v<uint16_t>) {
        m_buffer.bindIndexBuffer(m_boundIndexBuffer, 0, vk::IndexType::eUint16);
      }
      else {
        m_buffer.bindIndexBuffer(m_boundIndexBuffer, 0, vk::IndexType::eUint32);
      }
    }
  }

private:
  template <typename Pipeline> void bindGraphicPipeline(Pipeline &pipeline) {
    if (m_boundGraphicPipeline != pipeline.getHandle()) {
      m_boundGraphicPipeline = pipeline.getHandle();
      m_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                            m_boundGraphicPipeline);
    }
  }

private:
  vk::CommandBuffer m_buffer;
  vk::Pipeline m_boundGraphicPipeline{};
  std::array<vk::Buffer, MAX_BINDING> m_boundVertexBuffers{};
  vk::Buffer m_boundIndexBuffer{};
};
} // namespace phx
