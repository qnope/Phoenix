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

constexpr std::size_t MAX_STAGING_BUFFER_SIZE = 1 << 25;

template <vk::ImageViewType Type, vk::Format Format, VkImageUsageFlags _Usage>
class ImageLoader<SampledImageRef<Type, Format, _Usage>> {
  static constexpr auto Usage = _Usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

  using Image = Image<imageTypeFromImageViewType<Type>(), Format, Usage>;
  using ImageView = ImageView<Type, Format, Usage>;
  using SampledImage = SampledImageRef<Type, Format, _Usage>;

  using Buffer = StagingBuffer<std::byte>;

public:
  ImageLoader(Device &device) noexcept : m_device{device} {}

  SampledImage load(const std::string &path, bool withMipmap,
                    vk::PipelineStageFlags pipelineStage) {
    if (auto *image = ltl::find_map_ptr(m_images, path)) {
      return {(*image)[1_n], m_sampler};
    }
    auto [width, height, data] = loadImage(path);
    auto &buffer = getCandidateBuffer(data.size());
    auto [offset, size] = fillBuffer(buffer, data);
    auto &[image, imageView] = createNewImage(width, height, withMipmap, path);
    auto subResourceRange = vk::ImageSubresourceRange(
        image.aspectMask, 0, VK_REMAINING_MIP_LEVELS, 0, 1);

    LayoutTransitionUndefinedToTransferDstBarrier toTransferBarrier{
        image.getHandle(), subResourceRange};

    m_memoryTransfer.applyBarrier(toTransferBarrier);
    copyBufferToImage(buffer, image, offset);

    if (withMipmap) {
      generateMipmap(image);
      LayoutTransitionTransferSrcToSampledBarrier toSampledBarrier{
          image.getHandle(), pipelineStage, subResourceRange};
      m_memoryTransfer.applyBarrier(toSampledBarrier);
    } else {
      LayoutTransitionTransferDstToSampledBarrier toSampledBarrier{
          image.getHandle(), pipelineStage, subResourceRange};
      m_memoryTransfer.applyBarrier(toSampledBarrier);
    }

    return {imageView, m_sampler};
  }

  void flush() { m_memoryTransfer.flush(); }

private:
  void generateMipmap(const Image &image) {
    vk::ImageSubresourceRange firstLevelRange(image.aspectMask, 0, 1, 0, 1);
    LayoutTransitionTransferDstToSrcBarrier firstLevelBarrier{image.getHandle(),
                                                              firstLevelRange};
    m_memoryTransfer.applyBarrier(firstLevelBarrier);
    m_memoryTransfer.generateMipmap(image);
  }

  void copyBufferToImage(Buffer &buffer, Image &image, std::size_t offset) {
    vk::BufferImageCopy copier{};
    copier.imageExtent = image.getExtent();

    copier.bufferOffset = offset;

    copier.imageSubresource.layerCount = 1;
    copier.imageSubresource.aspectMask = image.aspectMask;
    m_memoryTransfer.copyBufferToImage(buffer, image, copier);
  }

  ltl::tuple_t<Image, ImageView> &createNewImage(Width width, Height height,
                                                 bool withMipmap,
                                                 const std::string &path) {
    auto w = width.get();
    auto h = height.get();
    auto image = m_device.createImage<Image>(w, h, 1u, withMipmap);
    auto imageView = image.template createImageView<Type>();
    auto [it, ok] = m_images.emplace(
        path,
        ltl::tuple_t<Image, ImageView>{std::move(image), std::move(imageView)});
    assert(ok);
    return it->second;
  }

  ltl::tuple_t<std::size_t, std::size_t>
  fillBuffer(Buffer &buffer, const std::vector<std::byte> &datas) {
    auto offset = buffer.sizeInBytes();
    for (auto x : datas)
      buffer << x;
    return {offset, buffer.sizeInBytes()};
  }

  auto &getCandidateBuffer(std::size_t size) {
    auto hasEnoughSize = [size](auto &buffer) {
      return buffer.size() + size < buffer.capacity();
    };

    if (auto *buffer = ltl::find_if_ptr(m_buffers, hasEnoughSize)) {
      return *buffer;
    }

    return m_buffers.emplace_back(
        m_device.createBuffer<Buffer>(std::max(size, MAX_STAGING_BUFFER_SIZE)));
  }

private:
  Device &m_device;
  MemoryTransfer m_memoryTransfer{m_device};
  Sampler m_sampler = m_device.createSampler(vk::Filter::eLinear,
                                             vk::SamplerMipmapMode::eLinear);

  std::vector<Buffer> m_buffers;
  std::unordered_map<std::string, ltl::tuple_t<Image, ImageView>> m_images;
};
} // namespace phx
