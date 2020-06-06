#pragma once

#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/Image/ImageLoader.h>

#include "Material.h"

namespace phx {
class Device;

class MaterialFactory {
public:
  MaterialFactory(Device &device) noexcept;

  Material createTexturedLambertianMaterial(const std::string &path) noexcept;

  void flush() noexcept;

  DescriptorPoolManager &descriptorPoolManager() noexcept;

private:
  ImageLoader<SampledImage2dRgbaSrgbRef> m_imageLoader;
  DescriptorPoolManager m_descriptorPoolManager;
};

} // namespace phx
