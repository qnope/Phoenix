#pragma once

#include <array>
#include "vulkan.h"
#include <ltl/Tuple.h>
#include <ltl/condition.h>

namespace phx {
class CommandBufferWrapper {
  public:
    explicit CommandBufferWrapper(vk::CommandBuffer buffer) noexcept : m_cmdBuffer{buffer} {}

    void bindVertexBuffer(vk::Buffer buffer) noexcept {
        if (buffer != m_boundVertexBuffer) {
            m_boundVertexBuffer = buffer;
            m_cmdBuffer.bindVertexBuffers(0, {m_boundVertexBuffer}, vk::DeviceSize(0));
        }
    }

    void bindIndexBuffer(vk::Buffer buffer) {
        if (m_boundIndexBuffer != buffer) {
            m_boundIndexBuffer = buffer;
            m_cmdBuffer.bindIndexBuffer(m_boundIndexBuffer, 0, vk::IndexType::eUint32);
        }
    }

    void bindDescriptorSet(vk::PipelineLayout pipelineLayout, vk::PipelineBindPoint bindingPoint, uint32_t setIndex,
                           vk::DescriptorSet set) {
        assert(setIndex < 4);
        if (m_boundSets[setIndex] != ltl::tuple_t{pipelineLayout, set}) {
            m_boundSets[setIndex] = ltl::tuple_t{pipelineLayout, set};
            m_cmdBuffer.bindDescriptorSets(bindingPoint, pipelineLayout, setIndex, set, nullptr);
        }
    }

    void bindGraphicPipeline(vk::Pipeline pipeline) {
        if (m_boundGraphicPipeline != pipeline) {
            m_boundGraphicPipeline = pipeline;
            m_cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_boundGraphicPipeline);
        }
    }

    void push(vk::PipelineLayout layout, vk::PushConstantRange range, const void *data) {
        m_cmdBuffer.pushConstants(layout, range.stageFlags, range.offset, range.size, data);
    }

  private:
    vk::CommandBuffer m_cmdBuffer;
    vk::Pipeline m_boundGraphicPipeline{};
    std::array<ltl::tuple_t<vk::PipelineLayout, vk::DescriptorSet>, 4> m_boundSets{};
    vk::Buffer m_boundVertexBuffer{};
    vk::Buffer m_boundIndexBuffer{};
};
} // namespace phx
