#pragma once

#include "../Device.h"
#include <ltl/TypedTuple.h>
#include <ltl/algos.h>

#include "../MemoryTransfer.h"
#include "../utility.h"
#include "Image.h"
#include "SampledImage.h"

namespace phx {
template <vk::ImageViewType type> constexpr auto imageTypeFromImageViewType() {
  if constexpr (type == vk::ImageViewType::e2D)
    return vk::ImageType::e2D;
}

template <typename SampledImage> class ImageLoader;

constexpr auto MAX_STAGING_BUFFER_SIZE = 1 << 25;

template <vk::ImageViewType Type, vk::Format Format, VkImageUsageFlags _Usage>
class ImageLoader<SampledImageRef<Type, Format, _Usage>> {
  static constexpr auto Usage = _Usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  using Image = Image<imageTypeFromImageViewType<Type>(), Format, Usage>;
  using ImageView = ImageView<Type, Format, Usage>;
  using SampledImage = SampledImageRef<Type, Format, _Usage>;

  using Buffer = StagingBuffer<std::byte>;

public:
  ImageLoader(Device &device) noexcept : m_device{device} {}

  SampledImage load(const std::string &path,
                    vk::PipelineStageFlags pipelineStage) {
    auto [width, height, data] = loadImage(path);
    auto &buffer = getCandidateBuffer(data.size());
    auto [offset, size] = fillBuffer(buffer, data);
    auto &[image, imageView] = createNewImage(width, height);
    auto subResourceRange =
        vk::ImageSubresourceRange(image.aspectMask, 0, 1, 0, 1);

    LayoutTransitionUndefinedToTransferSrcBarrier toTransferBarrier{
        image.getHandle(), subResourceRange};

    LayoutTransitionTransferToSampledBarrier toSampledBarrier{
        image.getHandle(), pipelineStage, subResourceRange};

    m_memoryTransfer.applyBarrier(toTransferBarrier);
    copyBufferToImage(buffer, image, offset);
    m_memoryTransfer.applyBarrier(toSampledBarrier);

    return {imageView, m_sampler};
  }

  void flush() { m_memoryTransfer.flush(); }

private:
  void copyBufferToImage(Buffer &buffer, Image &image, std::size_t offset) {
    vk::BufferImageCopy copier{};
    copier.imageExtent = image.getExtent();

    copier.bufferOffset = offset;

    copier.imageSubresource.layerCount = 1;
    copier.imageSubresource.aspectMask = image.aspectMask;
    m_memoryTransfer.copyBufferToImage(buffer, image, copier);
  }

  ltl::tuple_t<Image, ImageView> &createNewImage(Width width, Height height) {
    auto w = width.get();
    auto h = height.get();
    auto image = m_device.createImage<Image>(w, h, 1u);
    auto imageView = image.template createImageView<Type>();
    return m_images.emplace_back(std::move(image), std::move(imageView));
  }

  ltl::tuple_t<std::size_t, std::size_t>
  fillBuffer(Buffer &buffer, const std::vector<std::byte> &datas) {
    auto offset = buffer.size();
    for (auto x : datas)
      buffer << x;
    return {offset, buffer.size()};
  }

  auto &getCandidateBuffer(std::size_t size) {
    auto hasEnoughSize = [size](auto &buffer) {
      return buffer.size() + size < buffer.capacity();
    };

    if (auto *buffer = ltl::find_if_ptr(m_buffers, hasEnoughSize)) {
      return *buffer;
    }

    return m_buffers.emplace_back(
        m_device.createBuffer<Buffer>(ltl::max(size, MAX_STAGING_BUFFER_SIZE)));
  }

private:
  Device &m_device;
  MemoryTransfer m_memoryTransfer{m_device};
  Sampler m_sampler = m_device.createSampler(vk::Filter::eLinear,
                                             vk::SamplerMipmapMode::eLinear);

  std::vector<Buffer> m_buffers;
  std::vector<ltl::tuple_t<Image, ImageView>> m_images;
};
} // namespace phx
