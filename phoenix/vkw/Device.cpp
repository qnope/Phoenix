#include "Device.h"
#include "utility.h"
#include <ltl/range.h>
#include <ltl/smart_iterator.h>

namespace phx {
static auto getNeededDeviceExtensions() noexcept {
  std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  return deviceExtensions;
}

static bool isSwapchainSupported(vk::PhysicalDevice device,
                                 vk::SurfaceKHR surface) noexcept {
  auto formats = device.getSurfaceFormatsKHR(surface);
  auto presentModes = device.getSurfacePresentModesKHR(surface);
  return !formats.empty() && !presentModes.empty();
}

static bool isDeviceSuitable(vk::PhysicalDevice device,
                             vk::SurfaceKHR surface) noexcept {
  auto neededExtensions = getNeededDeviceExtensions();
  auto neededExtensionsString = to_string_vector(neededExtensions);

  if (!areAvailable(neededExtensionsString, device))
    return false;

  if (!isSwapchainSupported(device, surface))
    return false;

  auto deviceProperty = device.getProperties();

  return deviceProperty.deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
}

static vk::PhysicalDevice choosePhysicalDevice(vk::Instance instance,
                                               vk::SurfaceKHR surface) {
  auto physicalDevices = instance.enumeratePhysicalDevices();

  for (auto physicalDevice : physicalDevices) {
    if (isDeviceSuitable(physicalDevice, surface))
      return physicalDevice;
  }

  throw NoDeviceCompatibleException{};
}

static bool isQueueSuitable(vk::PhysicalDevice device,
                            std::size_t indexQueueFamily,
                            vk::QueueFamilyProperties property,
                            vk::SurfaceKHR surface) noexcept {
  constexpr auto computeGraphicBits =
      vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute;
  const bool supportComputeGraphicOperations =
      (property.queueFlags & computeGraphicBits) == computeGraphicBits;
  const bool supportPresentationOperation = device.getSurfaceSupportKHR(
      static_cast<uint32_t>(indexQueueFamily), surface);

  return supportComputeGraphicOperations && supportPresentationOperation;
}

static uint32_t getQueueFamily(vk::PhysicalDevice device,
                               vk::SurfaceKHR surface) {
  auto queueFamilies = device.getQueueFamilyProperties();

  for (auto [indexQueueFamily, queueFamilyProperty] :
       ltl::enumerate(queueFamilies)) {
    if (isQueueSuitable(device, indexQueueFamily, queueFamilyProperty,
                        surface)) {
      return static_cast<uint32_t>(indexQueueFamily);
    }
  }

  throw NoGraphicComputeQueueException{};
}

static auto createDeviceQueueInfo(uint32_t familyIndex) noexcept {
  vk::DeviceQueueCreateInfo info;
  static constexpr float queuePriority = 1.0f;
  info.queueCount = 1;
  info.queueFamilyIndex = familyIndex;
  info.pQueuePriorities = &queuePriority;
  return info;
}

static constexpr auto createDeviceFeatures() noexcept {
  vk::PhysicalDeviceFeatures features;
  return features;
}

Device::Device(const Instance &instance, const Surface &surface) {
  m_physicalDevice =
      choosePhysicalDevice(instance.getHandle(), surface.getHandle());
  auto queueFamily = getQueueFamily(m_physicalDevice, surface.getHandle());
  auto queueInfo = createDeviceQueueInfo(queueFamily);
  constexpr auto features = createDeviceFeatures();

  const auto &layers = instance.getValidationLayers();
  const auto extensions = getNeededDeviceExtensions();

  vk::DeviceCreateInfo info;
  info.queueCreateInfoCount = 1;
  info.pQueueCreateInfos = &queueInfo;
  info.pEnabledFeatures = &features;
  info.enabledLayerCount = static_cast<uint32_t>(layers.size());
  info.ppEnabledLayerNames = layers.data();
  info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  info.ppEnabledExtensionNames = extensions.data();
  m_handle = m_physicalDevice.createDeviceUnique(info);
  m_queue =
      std::make_unique<Queue>(m_handle->getQueue(queueFamily, 0), queueFamily);
}

Queue &Device::getQueue() const noexcept { return *m_queue; }

vk::PhysicalDevice Device::getPhysicalDevice() const noexcept {
  return m_physicalDevice;
}

} // namespace phx
