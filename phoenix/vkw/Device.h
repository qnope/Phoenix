#pragma once
#include "Instance.h"

namespace phx {
struct NoDeviceCompatibleException {};
struct NoGraphicComputeQueueException {};

class Device final {
public:
  Device(Instance &instance);

private:
  vk::UniqueDevice m_device;
};
} // namespace phx
