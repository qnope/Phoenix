#pragma once

#include <vkw/Descriptor/DescriptorSetLayout.h>
#include <vkw/PipelineLayout.h>
#include "AbstractMaterial.h"
#include "BufferedMaterialManager.h"

namespace phx {
using ColoredLambertianMaterialManager = BufferedMaterialManager<glm::vec4>;
using ColoredLambertianMaterial = BufferedMaterial<ColoredLambertianMaterialManager>;
} // namespace phx
