#include <vkw/Device.h>

#include "MaterialFactory.h"

#include "TexturedLambertianMaterial.h"

namespace phx {

MaterialFactory::MaterialFactory(Device &device) noexcept
    : m_imageLoader{device} {
  prepareDescriptorPools(device);
}

void MaterialFactory::prepareDescriptorPools(Device &device) {
  m_descriptorPoolManager.addDescriptorPool(
      device.createDescriptorPool<TexturedLambertianMaterialSetLayout>());
}

Material MaterialFactory::createTexturedLambertianMaterial(
    const std::string &path) noexcept {
  return TexturedLambertianMaterial(path, m_imageLoader,
                                    m_descriptorPoolManager);
}

} // namespace phx
