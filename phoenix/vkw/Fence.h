#ifndef FENCE_H
#define FENCE_H

#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {

class Fence : public VulkanResource<vk::UniqueFence> {
public:
  Fence(vk::Device device, bool signaledState) noexcept;

  void reset() noexcept;
  void wait(uint64_t timeout = VK_WHOLE_SIZE) noexcept;
  void waitAndReset() noexcept;

private:
  vk::Device m_device;
};

} // namespace phx

#endif // FENCE_H
