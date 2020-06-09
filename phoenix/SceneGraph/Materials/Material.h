#pragma once

#include <typeindex>
#include <utility>
#include <vkw/vulkan.h>

#include <ltl/movable_any.h>

#include <vkw/Descriptor/DescriptorSet.h>

namespace phx {
class Material {
public:
  template <typename T>
  Material(T material) noexcept
      : m_layoutType{material.layoutType()},       //
        m_descriptorSet{material.descriptorSet()}, //
        m_material{std::move(material)} {}

  std::type_index type() const noexcept { return m_material.type(); }
  std::type_index layoutType() const noexcept { return m_layoutType; }
  DescriptorSet descriptorSet() const noexcept { return m_descriptorSet; }

  template <typename T> T &get() { return m_material.get<T>(); }

private:
  std::type_index m_layoutType;
  DescriptorSet m_descriptorSet;
  ltl::movable_any m_material;
};
} // namespace phx
