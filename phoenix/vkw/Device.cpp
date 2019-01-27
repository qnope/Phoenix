#include <ltl/smart_iterator.h>

#include "Device.h"

namespace phx {
static vk::PhysicalDevice choosePhysicalDevice(Instance &instance) {
  auto physicalDevices = instance.physicalDevices();

  for (auto physicalDevice : physicalDevices) {
    auto deviceProperty = physicalDevice.getProperties();
    if (deviceProperty.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
      return physicalDevice;
    }
  }

  throw NoDeviceCompatibleException{};
}

static uint32_t getQueueFamily(vk::PhysicalDevice device) {
  auto queueFamilies = device.getQueueFamilyProperties();
  constexpr auto queueOperations =
      vk::QueueFlagBits::eCompute | vk::QueueFlagBits::eGraphics;

  for (auto [i, queueFamily] : ltl::enumerate(queueFamilies)) {
    if (queueFamily.queueCount > 0 &&
        (queueFamily.queueFlags & queueOperations) == queueOperations) {
      return static_cast<uint32_t>(i);
    }
  }

  throw NoGraphicComputeQueueException{};
}

static auto createDeviceQueueInfo(uint32_t familyIndex) {
  vk::DeviceQueueCreateInfo info;
  static constexpr float queuePriority = 1.0f;
  info.queueCount = 1;
  info.queueFamilyIndex = familyIndex;
  info.pQueuePriorities = &queuePriority;
  return info;
}

static constexpr auto createDeviceFeatures() {
  vk::PhysicalDeviceFeatures features;
  return features;
}

Device::Device(Instance &instance) {
  auto physicalDevice = choosePhysicalDevice(instance);
  auto queueFamily = getQueueFamily(physicalDevice);
  auto queueInfo = createDeviceQueueInfo(queueFamily);
  constexpr auto features = createDeviceFeatures();

  const auto &layers = instance.validationLayers();

  vk::DeviceCreateInfo info;
  info.queueCreateInfoCount = 1;
  info.pQueueCreateInfos = &queueInfo;
  info.pEnabledFeatures = &features;
  info.enabledLayerCount = static_cast<uint32_t>(layers.size());
  info.ppEnabledLayerNames = layers.data();
  m_device = physicalDevice.createDeviceUnique(info);
  m_queue = std::make_unique<Queue>(m_device->getQueue(queueFamily, 0));
}

Queue &Device::getQueue() { return *m_queue; }

} // namespace phx
