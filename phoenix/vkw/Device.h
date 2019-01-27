#pragma once

#include <memory>

#include "Instance.h"
#include "Queue.h"

namespace phx {
struct NoDeviceCompatibleException {};
struct NoGraphicComputeQueueException {};

class Device final {
public:
  Device(Instance &instance);

  Queue &getQueue();

private:
  vk::UniqueDevice m_device;
  std::unique_ptr<Queue> m_queue;
};
} // namespace phx
