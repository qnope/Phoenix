#include "Swapchain.h"
#include <limits>
namespace phx {

static vk::SurfaceFormatKHR chooseSwapchainFormat(vk::PhysicalDevice device,
                                                  vk::SurfaceKHR surface) {
  auto formats = device.getSurfaceFormatsKHR(surface);

  if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
    return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
  }

  for (auto format : formats) {
    if (format.format == vk::Format::eB8G8R8A8Unorm &&
        format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
      return format;
    }
  }

  throw NoFormatAvailableException{};
}

static vk::PresentModeKHR
chooseSwapchainPresentMode(vk::PhysicalDevice device,
                           vk::SurfaceKHR surface) noexcept {
  auto presentModes = device.getSurfacePresentModesKHR(surface);
  vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

  for (auto presentMode : presentModes) {
    if (presentMode == vk::PresentModeKHR::eMailbox)
      return presentMode;
    else if (presentMode == vk::PresentModeKHR::eImmediate)
      bestMode = presentMode;
  }

  return bestMode;
}

static vk::Extent2D chooseSwapchainExtent(vk::PhysicalDevice device,
                                          vk::SurfaceKHR surface, Width width,
                                          Height height) noexcept {
  auto capabilities = device.getSurfaceCapabilitiesKHR(surface);

  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    return capabilities.currentExtent;

  const vk::Extent2D minExtent = capabilities.minImageExtent;
  const vk::Extent2D maxExtent = capabilities.maxImageExtent;
  const uint32_t w = width.get();
  const uint32_t h = height.get();

  vk::Extent2D extent;
  extent.width = std::clamp(w, minExtent.width, maxExtent.width);
  extent.height = std::clamp(h, minExtent.height, maxExtent.height);
  return extent;
}

Swapchain::Swapchain(Device &device, Surface &surface, Width width,
                     Height height) {
  using namespace vk;
  auto physicalDevice = device.getPhysicalDevice();
  auto surfaceKHR = surface.getHandle();
  auto format = chooseSwapchainFormat(physicalDevice, surfaceKHR);
  auto presentMode = chooseSwapchainPresentMode(physicalDevice, surfaceKHR);
  auto size = chooseSwapchainExtent(physicalDevice, surfaceKHR, width, height);

  auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surfaceKHR);
  auto imageCount =
      std::clamp(3u, capabilities.minImageCount, capabilities.maxImageCount);

  SwapchainCreateInfoKHR info;

  info.surface = surfaceKHR;
  info.minImageCount = imageCount;
  info.imageFormat = format.format;
  info.imageColorSpace = format.colorSpace;
  info.imageExtent = size;
  info.imageArrayLayers = 1;
  info.imageUsage = ImageUsageFlagBits::eColorAttachment;
  info.imageSharingMode = SharingMode::eExclusive;
  info.preTransform = capabilities.currentTransform;
  info.compositeAlpha = CompositeAlphaFlagBitsKHR::eOpaque;
  info.presentMode = presentMode;
  info.clipped = true;

  vk::Device deviceHandle = device.getHandle();
  m_handle = deviceHandle.createSwapchainKHRUnique(info);

  auto images = deviceHandle.getSwapchainImagesKHR(*m_handle);

  Extent3D extent(size.width, size.height, 1);

  for (auto vkimage : images) {
    ImageSubresourceRange range(ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    SwapchainImage image{deviceHandle, vkimage, format.format, extent, 1u, 1u};
    auto imageView =
        image.createImageView(ImageViewType::e2D, format.format, range);

    m_swapchainImages.emplace_back(std::move(image), std::move(imageView));
  }
}

} // namespace phx
