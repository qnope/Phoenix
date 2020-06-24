#include <vkw/Device.h>

#include "MaterialFactory.h"

#include "TexturedLambertianMaterial.h"
#include "ColoredLambertianMaterial.h"

namespace phx {

MaterialFactory::MaterialFactory(Device &device) noexcept :
    m_device{device},      //
    m_imageLoader{device}, //
    m_descriptorPoolManager{device} {}

Material MaterialFactory::createTexturedLambertianMaterial(const std::string &path) noexcept {
    return m_texturedLambertianMaterialManager.allocate({path}, true, vk::PipelineStageFlagBits::eFragmentShader);
}

Material MaterialFactory::createColoredLambertianMaterial(glm::vec4 albedo) noexcept {
    return m_coloredLambertianMaterialManager.allocate(albedo);
}

void MaterialFactory::flush() noexcept {
    m_imageLoader.flush();
    m_coloredLambertianMaterialManager.flush();
}

} // namespace phx
