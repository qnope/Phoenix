#pragma once

#include "../VulkanResource.h"
#include "../vulkan.h"

namespace phx {
class Sampler : public VulkanResource<vk::UniqueSampler> {
  public:
    Sampler(vk::Device device, vk::Filter minMag, vk::SamplerMipmapMode mipmapMode) noexcept;
};
} // namespace phx
