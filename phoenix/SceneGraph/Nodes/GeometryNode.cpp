#include "GeometryNode.h"

namespace phx {

GeometryNode::GeometryNode(DrawInformations informations, Material material) noexcept :
    m_informations{informations}, //
    m_material{std::move(material)} {}

DrawInformations GeometryNode::drawInformations() const noexcept { return m_informations; }

Material GeometryNode::material() const noexcept { return m_material; }

} // namespace phx
