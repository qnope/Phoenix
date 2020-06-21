#include <vkw/Device.h>

#include "MaterialFactory.h"

#include "TexturedLambertianMaterial.h"
#include "ColoredLambertianMaterial.h"

namespace phx {

MaterialFactory::MaterialFactory(Device &device) noexcept :
    m_imageLoader{device},   //
    m_bufferManager{device}, //
    m_descriptorPoolManager{device} {}

Material MaterialFactory::createTexturedLambertianMaterial(const std::string &path) noexcept {
    return TexturedLambertianMaterial(path, m_imageLoader, m_descriptorPoolManager);
}

Material MaterialFactory::createColoredLambertianMaterial(glm::vec4 albedo) noexcept {
    auto [descriptorSet, index] = sendToStorageBuffer(albedo);

    return ColoredLambertianMaterial{descriptorSet, index};
}

void MaterialFactory::flush() noexcept {
    m_bufferManager.flush(vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eShaderRead);
    m_imageLoader.flush();
}

DescriptorPoolManager &MaterialFactory::descriptorPoolManager() noexcept { return m_descriptorPoolManager; }

template <typename T>
ltl::tuple_t<DescriptorSet, uint32_t> MaterialFactory::sendToStorageBuffer(T value) noexcept {
    auto bufferInfo = m_bufferManager.send<T, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT>(std::vector{value});

    auto bufferHandle = bufferInfo.buffer.getHandle();

    if (auto descriptorSet = ltl::find_map_value(m_bufferMaterialDescriptorSets, bufferHandle)) {
        return {*descriptorSet, uint32_t(bufferInfo.offset)};
    }

    using Binding = DescriptorBinding<VK_SHADER_STAGE_FRAGMENT_BIT, vk::DescriptorType::eStorageBuffer, 1, T>;
    using Layout = DescriptorSetLayout<Binding>;

    return {m_descriptorPoolManager.allocate<Layout>({bufferInfo.buffer}), uint32_t(bufferInfo.offset)};
}

} // namespace phx
