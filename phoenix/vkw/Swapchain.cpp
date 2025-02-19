#include "Swapchain.h"
#include <limits>

#include <ltl/Range/DefaultView.h>

namespace phx {

static vk::SurfaceFormatKHR chooseSwapchainFormat(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
    auto formats = device.getSurfaceFormatsKHR(surface);

    if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
        return {vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear};
    }

    for (auto format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }

    throw NoFormatAvailableException{};
}

static vk::PresentModeKHR chooseSwapchainPresentMode(vk::PhysicalDevice device, vk::SurfaceKHR surface) noexcept {
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

static vk::Extent2D chooseSwapchainExtent(vk::PhysicalDevice device, vk::SurfaceKHR surface, Width width,
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

Swapchain::Swapchain(Device &device, Surface &surface, Width width, Height height) : m_device{device.getHandle()} {
    using namespace vk;
    auto physicalDevice = device.getPhysicalDevice();
    auto surfaceKHR = surface.getHandle();
    m_surfaceFormat = chooseSwapchainFormat(physicalDevice, surfaceKHR);
    auto presentMode = chooseSwapchainPresentMode(physicalDevice, surfaceKHR);
    auto size = chooseSwapchainExtent(physicalDevice, surfaceKHR, width, height);

    auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surfaceKHR);
    m_imageCount = std::clamp(3u, capabilities.minImageCount, capabilities.maxImageCount);

    SwapchainCreateInfoKHR info;

    info.surface = surfaceKHR;
    info.minImageCount = m_imageCount;
    info.imageFormat = m_surfaceFormat.format;
    info.imageColorSpace = m_surfaceFormat.colorSpace;
    info.imageExtent = size;
    info.imageArrayLayers = 1;
    info.imageUsage = ImageUsageFlagBits::eColorAttachment;
    info.imageSharingMode = SharingMode::eExclusive;
    info.preTransform = capabilities.currentTransform;
    info.compositeAlpha = CompositeAlphaFlagBitsKHR::eOpaque;
    info.presentMode = presentMode;
    info.clipped = true;

    m_handle = m_device.createSwapchainKHRUnique(info);

    auto images = m_device.getSwapchainImagesKHR(*m_handle);

    m_extent = vk::Extent3D{size.width, size.height, 1};

    for (auto vkimage : images) {
        SwapchainImage image{m_device, vkimage, m_extent, 1u, 1u};
        auto imageView = image.createImageView<ImageViewType::e2D>();

        m_swapchainImages.emplace_back(std::move(image), std::move(imageView));
    }
}

vk::Format Swapchain::getImageFormat() const noexcept { return m_surfaceFormat.format; }

uint32_t Swapchain::getImageCount() const noexcept { return m_imageCount; }

vk::AttachmentDescription Swapchain::getAttachmentDescription() const noexcept {
    vk::AttachmentDescription description;

    description.initialLayout = vk::ImageLayout::eUndefined;
    description.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    description.format = getImageFormat();

    description.loadOp = vk::AttachmentLoadOp::eClear;
    description.storeOp = vk::AttachmentStoreOp::eStore;

    description.samples = vk::SampleCountFlagBits::e1;

    description.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    description.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

    return description;
}

void Swapchain::generateFramebuffer(vk::RenderPass renderpass) noexcept {
    using namespace ltl;
    for (auto &imgView : m_swapchainImages | ltl::get(1_n)) {
        m_framebuffers.emplace_back(Framebuffer{m_device, renderpass, m_extent.width, m_extent.height, imgView});
    }
}

const Framebuffer<1> &Swapchain::getFramebuffer(uint32_t index) const noexcept { return m_framebuffers[index]; }

const std::vector<Framebuffer<1>> &Swapchain::getFramebuffers() const noexcept { return m_framebuffers; }

} // namespace phx
