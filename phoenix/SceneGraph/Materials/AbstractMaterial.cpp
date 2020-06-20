#include "AbstractMaterial.h"

namespace phx {
AbstractMaterial::AbstractMaterial(DescriptorSet descriptorSet) noexcept : m_descriptorSet{descriptorSet} {}

std::type_index AbstractMaterial::layoutType() const noexcept { return m_descriptorSet.layoutType(); }

DescriptorSet AbstractMaterial::descriptorSet() const noexcept { return m_descriptorSet; }

} // namespace phx
