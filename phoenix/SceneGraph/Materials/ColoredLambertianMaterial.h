#pragma once

#include "AbstractMaterial.h"

namespace phx {

class ColoredLambertianMaterial : public AbstractMaterial {
public:
  ColoredLambertianMaterial(glm::vec4 color);
};

} // namespace phx
