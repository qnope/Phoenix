#pragma once

#include <memory>

#include "Instance.h"
#include "Queue.h"
#include "Surface.h"
#include "VulkanResource.h"

namespace phx {
struct NoDeviceCompatibleException {};
struct NoGraphicComputeQueueException {};

class Device final : public VulkanResource<vk::UniqueDevice> {
public:
  Device(const Instance &instance, const Surface &surface);

  Queue &getQueue();

private:
  std::unique_ptr<Queue> m_queue;
};
} // namespace phx
