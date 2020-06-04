#pragma once

#include "AbstractMaterial.h"
#include <vkw/Descriptor/DescriptorBinding.h>
#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/Descriptor/DescriptorSetLayout.h>
#include <vkw/Image/ImageLoader.h>

namespace phx {

using TexturedLambertianMaterialSetLayout =
    DescriptorSetLayout<SampledImage2dRgbaSrgbBinding>;

class TexturedLambertianMaterial : public AbstractMaterial {
public:
  TexturedLambertianMaterial(
      const std::string &view,
      ImageLoader<SampledImage2dRgbaSrgbRef> &imageLoader,
      DescriptorPoolManager &pool) noexcept;
};

} // namespace phx
