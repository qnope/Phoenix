#include <vkw/Device.h>

#include "MaterialFactory.h"

#include "TexturedLambertianMaterial.h"

namespace phx {

MaterialFactory::MaterialFactory(Device &device) noexcept :
    m_imageLoader{device}, //
    m_descriptorPoolManager{device} {}

Material MaterialFactory::createTexturedLambertianMaterial(const std::string &path) noexcept {
    return TexturedLambertianMaterial(path, m_imageLoader, m_descriptorPoolManager);
}

void MaterialFactory::flush() noexcept { return m_imageLoader.flush(); }

DescriptorPoolManager &MaterialFactory::descriptorPoolManager() noexcept { return m_descriptorPoolManager; }

} // namespace phx
