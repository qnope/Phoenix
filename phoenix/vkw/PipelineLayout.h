#pragma once

#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {
template <typename... Uniforms>
class PipelineLayout : public VulkanResource<vk::UniquePipelineLayout> {
public:
  PipelineLayout(vk::Device device, Uniforms... uniforms) {
    vk::PipelineLayoutCreateInfo info;
    m_handle = device.createPipelineLayoutUnique(info);
  }

private:
};
} // namespace phx
