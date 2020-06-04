#pragma once

#include <typeindex>
#include <vkw/vulkan.h>

namespace phx {
class AbstractMaterial {
protected:
  AbstractMaterial(std::type_index layoutType,
                   vk::DescriptorSet descriptorSet) noexcept;

public:
  std::type_index layoutType() const noexcept;
  vk::DescriptorSet descriptorSet() const noexcept;

private:
  std::type_index m_layoutType;
  vk::DescriptorSet m_descriptorSet;
};
} // namespace phx
