#include "AbstractMaterial.h"

namespace phx {
AbstractMaterial::AbstractMaterial(std::type_index index,
                                   vk::DescriptorSet descriptorSet) noexcept
    : m_layoutType{index}, m_descriptorSet{descriptorSet} {}

std::type_index AbstractMaterial::layoutType() const noexcept {
  return m_layoutType;
}

vk::DescriptorSet AbstractMaterial::descriptorSet() const noexcept {
  return m_descriptorSet;
}

} // namespace phx
