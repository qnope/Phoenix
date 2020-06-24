#pragma once

#include "AbstractMaterial.h"
#include <vkw/Descriptor/DescriptorBinding.h>
#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/Descriptor/DescriptorSetLayout.h>
#include <vkw/Image/ImageLoader.h>
#include <vkw/PipelineLayout.h>

#include "TexturedMaterialManager.h"

namespace phx {

using TexturedLambertianMaterialManager = TexturedMaterialManager<SampledImage2dRgbaSrgbRef, 1>;
using TexturedLambertianMaterial = TexturedMaterial<TexturedLambertianMaterialManager>;

} // namespace phx
