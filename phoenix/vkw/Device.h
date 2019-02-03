#pragma once

#include <memory>

#include "Instance.h"
#include "Queue.h"
#include "ShaderModule.h"
#include "Surface.h"
#include "VulkanResource.h"

namespace phx {
struct NoDeviceCompatibleException {};
struct NoGraphicComputeQueueException {};

class Device final : public VulkanResource<vk::UniqueDevice> {
public:
  Device(const Instance &instance, const Surface &surface);

  Queue &getQueue() const noexcept;

  vk::PhysicalDevice getPhysicalDevice() const noexcept;

  template <typename Type>
  ShaderModule<Type> createShaderModule(const std::string &path,
                                        bool debug) const {
    return ShaderModule<Type>(getHandle(), path, debug);
  }

private:
  vk::PhysicalDevice m_physicalDevice;
  std::unique_ptr<Queue> m_queue;
};
} // namespace phx
