#pragma once

#include "ImageView.h"
#include "Sampler.h"

namespace phx {

template <vk::ImageViewType Type, vk::Format Format, VkImageUsageFlags Usage>
class SampledImageRef {
public:
  static constexpr auto type = Type;
  static constexpr auto format = Format;
  static constexpr auto imageUsage = Usage;

  template <VkImageUsageFlags _Usage>
  SampledImageRef(const ImageView<Type, Format, _Usage> &imageView,
                  const Sampler &sampler) noexcept
      : imageView{imageView.getHandle()}, sampler{sampler.getHandle()} {
    static_assert((Usage & _Usage) == Usage, "Usage must corresponds");
  }

  vk::ImageView imageView;
  vk::Sampler sampler;
};
using SampledImageType =
    SampledImageRef<vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Unorm,
                    VK_IMAGE_USAGE_SAMPLED_BIT>;
} // namespace phx
