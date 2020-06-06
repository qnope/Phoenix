#pragma once

#include "../DrawInformations/DrawInformations.h"
#include "../Materials/Material.h"
#include "AbstractNode.h"

namespace phx {

class GeometryNode : public AbstractLeafNode {
public:
  GeometryNode(DrawInformations informations, Material material) noexcept;

  DrawInformations drawInformations() const noexcept;
  Material material() const noexcept;

private:
  DrawInformations m_informations;
  Material m_material;
};

} // namespace phx
