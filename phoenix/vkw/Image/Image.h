#pragma once

#include <ltl/condition.h>
#include <ltl/ltl.h>
#include <variant>

#include "ImageView.h"

#include "../Allocator/Allocator.h"

namespace phx {
template <vk::ImageType Type, vk::Format Format, VkImageUsageFlags Usage>
class Image {
public:
  static constexpr auto type = Type;
  static constexpr auto imageUsage = vk::ImageUsageFlags(Usage);
  static constexpr auto format = Format;

  static constexpr auto aspectMask =
      (format == ltl::AnyOf{vk::Format::eD16Unorm, vk::Format::eD32Sfloat})
          ? vk::ImageAspectFlagBits::eDepth
          : vk::ImageAspectFlagBits::eColor;

  Image(vk::Device device, vk::Image image, vk::Extent3D extent,
        uint32_t mipLevels, uint32_t arrayLayers) noexcept
      : m_device{device}, m_handle{image}, m_extent{extent},
        m_mipLevels{mipLevels}, m_arrayLayers{arrayLayers} {}

  Image(Allocator &allocator, uint32_t width, uint32_t height, uint32_t depth,
        VmaMemoryUsage memoryUsage)
      : m_allocator{&allocator}, m_device{allocator.device()}, m_extent{width,
                                                                        height,
                                                                        depth} {
    vk::ImageCreateInfo info;
    info.mipLevels = 1;
    info.usage = imageUsage;
    info.format = format;
    info.imageType = type;
    info.arrayLayers = 1;
    info.extent = m_extent;
    info.tiling = vk::ImageTiling::eOptimal;
    info.samples = vk::SampleCountFlagBits::e1;
    info.sharingMode = vk::SharingMode::eExclusive;
    info.initialLayout = vk::ImageLayout::eUndefined;

    ltl::tie(m_handle, m_allocation) =
        allocator.allocateImage(info, memoryUsage);
  }

  Image(Allocator &allocator, uint32_t width, uint32_t height,
        uint32_t depth) noexcept
      : Image(allocator, width, height, depth, VMA_MEMORY_USAGE_GPU_ONLY) {}

  Image(Image &&image) noexcept
      : m_allocator{image.m_allocator}, m_device{image.m_device},
        m_handle{image.m_handle}, m_extent{image.m_extent},
        m_mipLevels{image.m_mipLevels}, m_arrayLayers{image.m_arrayLayers},
        m_allocation{image.m_allocation} {
    image.m_allocator = nullptr;
    image.m_device = vk::Device();
    image.m_handle = vk::Image();
    image.m_extent = vk::Extent3D();
    image.m_mipLevels = 0;
    image.m_arrayLayers = 0;
    image.m_allocation = VmaAllocation();
  }

  Image &operator=(Image image) noexcept {
    deallocate();
    m_allocator = image.m_allocator;
    m_device = image.m_device;
    m_handle = image.m_handle;
    m_extent = image.m_extent;
    m_mipLevels = image.m_mipLevels;
    m_arrayLayers = image.m_arrayLayers;
    m_allocation = image.m_allocation;
    return *this;
  }

  vk::Extent3D getExtent() const noexcept { return m_extent; }
  uint32_t getMipLevels() const noexcept { return m_mipLevels; }
  uint32_t getArrayLayers() const noexcept { return m_arrayLayers; }

  auto getSubresourceRange() const noexcept {
    vk::ImageSubresourceRange range;
    range.aspectMask = aspectMask;
    range.levelCount = m_mipLevels;
    range.layerCount = m_arrayLayers;
    range.baseMipLevel = 0;
    range.baseArrayLayer = 0;
    return range;
  }

  vk::Image getHandle() const noexcept { return m_handle; }

  template <vk::ImageViewType ImageViewType>
  ImageView<ImageViewType, Format, Usage> createImageView() const noexcept {
    typed_static_assert_msg(
        (isImageTypeCompatibleWithImageViewType<Type, ImageViewType>()),
        "ImageViewType must be compatible with ImageType");
    return ImageView<ImageViewType, Format, Usage>{m_device, getHandle(),
                                                   getSubresourceRange()};
  }

  void deallocate() {
    if (m_allocator != nullptr) {
      m_allocator->deallocateImage(m_handle, m_allocation);
    }
  }

  ~Image() { deallocate(); }

private:
  Allocator *m_allocator = nullptr;
  vk::Device m_device;
  vk::Image m_handle;
  vk::Extent3D m_extent;
  uint32_t m_mipLevels = 1u;
  uint32_t m_arrayLayers = 1u;
  VmaAllocation m_allocation;
};

constexpr auto is_image = [](const auto &t) {
  auto hasImageUsage = IS_VALID((x), x.imageUsage);
  return hasImageUsage(t);
};
template <typename T>
constexpr auto IsImage = decltype(is_image(std::declval<T>()))::value;

using SampledImage2dRgbaSrgb =
    Image<vk::ImageType::e2D, vk::Format::eR8G8B8A8Srgb,
          VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
              VK_IMAGE_USAGE_TRANSFER_SRC_BIT>;

template <vk::ImageUsageFlagBits usage, typename I>
constexpr auto doesImageSupport(I &i) noexcept {
  typed_static_assert_msg(is_image(i), "i must be an image");
  return ltl::bool_v<(std::decay_t<I>::imageUsage & usage) == usage>;
}

template <typename I, requires_f(IsImage<I>)> auto getDescriptorInfo(I &i) {
  return vk::DescriptorImageInfo(i.sampler, i.imageView,
                                 vk::ImageLayout::eShaderReadOnlyOptimal);
}

} // namespace phx
