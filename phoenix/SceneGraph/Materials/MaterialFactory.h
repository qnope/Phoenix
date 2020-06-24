#pragma once

#include <unordered_map>
#include <vkw/Image/ImageLoader.h>
#include <vkw/Descriptor/DescriptorPoolManager.h>

#include "Material.h"
#include "ColoredLambertianMaterial.h"
#include "TexturedLambertianMaterial.h"

namespace phx {
class Device;

class MaterialFactory {
  public:
    MaterialFactory(Device &device) noexcept;

    Material createTexturedLambertianMaterial(const std::string &path) noexcept;
    Material createColoredLambertianMaterial(glm::vec4 albedo) noexcept;

    void flush() noexcept;

  private:
    Device &m_device;
    ImageLoader<SampledImage2dRgbaSrgbRef> m_imageLoader;
    DescriptorPoolManager m_descriptorPoolManager;

    TexturedLambertianMaterialManager m_texturedLambertianMaterialManager{m_imageLoader, m_descriptorPoolManager};
    ColoredLambertianMaterialManager m_coloredLambertianMaterialManager{m_device, m_descriptorPoolManager};
};

} // namespace phx
