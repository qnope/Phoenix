#pragma once

#include <typeindex>
#include <vkw/vulkan.h>

#include <vkw/Descriptor/DescriptorSet.h>

namespace phx {
class AbstractMaterial {
protected:
  AbstractMaterial(DescriptorSet descriptorSet) noexcept;

public:
  std::type_index layoutType() const noexcept;
  DescriptorSet descriptorSet() const noexcept;

private:
  DescriptorSet m_descriptorSet;
};
} // namespace phx
