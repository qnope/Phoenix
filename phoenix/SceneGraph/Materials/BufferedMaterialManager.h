#pragma once

#include <ltl/algos.h>
#include <unordered_map>
#include <vkw/PipelineLayout.h>
#include <vkw/Buffer/BufferList.h>
#include <vkw/Descriptor/DescriptorPoolManager.h>

#include "BufferedMaterial.h"

namespace phx {
template <typename T>
class BufferedMaterialManager {
    using Binding = DescriptorBinding<VK_SHADER_STAGE_FRAGMENT_BIT, vk::DescriptorType::eStorageBuffer, 1, T>;

  public:
    using Layout = DescriptorSetLayout<Binding>;

    BufferedMaterialManager(Device &device, DescriptorPoolManager &descriptorPoolManager) noexcept :
        m_descriptorPoolManager{descriptorPoolManager}, //
        m_buffers{device} {}

    BufferedMaterial<BufferedMaterialManager> allocate(const T &value) noexcept {
        auto bufferInfo = m_buffers.send(std::vector{value});
        auto bufferHandle = bufferInfo.buffer.getHandle();

        if (auto set = ltl::find_map_value(m_descriptorSets, bufferHandle)) {
            return {*set, uint32_t(bufferInfo.offset)};
        }

        return {m_descriptorPoolManager.allocate<Layout>({bufferInfo.buffer}), uint32_t(bufferInfo.offset)};
    }

    void flush() noexcept {
        m_buffers.flush(vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eShaderRead);
    }

  private:
    DescriptorPoolManager &m_descriptorPoolManager;
    BufferList<BufferInfo<T, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT>> m_buffers;
    std::unordered_map<vk::Buffer, DescriptorSet> m_descriptorSets;
};
} // namespace phx
