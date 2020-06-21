#pragma once

#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/Image/ImageLoader.h>
#include <vkw/Buffer/BufferManager.h>

#include "Material.h"
#include <unordered_map>

namespace phx {
class Device;

class MaterialFactory {
  public:
    MaterialFactory(Device &device) noexcept;

    Material createTexturedLambertianMaterial(const std::string &path) noexcept;
    Material createColoredLambertianMaterial(glm::vec4 albedo) noexcept;

    void flush() noexcept;

    DescriptorPoolManager &descriptorPoolManager() noexcept;

  private:
    template <typename T>
    ltl::tuple_t<DescriptorSet, uint32_t> sendToStorageBuffer(T value) noexcept;

  private:
    ImageLoader<SampledImage2dRgbaSrgbRef> m_imageLoader;
    BufferManager m_bufferManager;
    DescriptorPoolManager m_descriptorPoolManager;
    std::unordered_map<vk::Buffer, DescriptorSet> m_bufferMaterialDescriptorSets;
};

} // namespace phx
