#pragma once

#include "vulkan.hpp"
#include <ltl/overloader.h>
#include <variant>

namespace phx {

inline constexpr std::size_t colorAttachmentUsageBit() {
  return static_cast<std::size_t>(vk::ImageUsageFlagBits::eColorAttachment);
}

template <std::size_t Usage> class Image {
public:
  static constexpr std::size_t usage = Usage;

  Image(vk::Image image, vk::Format format, vk::Extent3D extent,
        uint32_t mipLevels, uint32_t arrayLayers) noexcept
      : m_handle{image}, m_format{format}, m_extent{extent},
        m_mipLevels{mipLevels}, m_arrayLayers{arrayLayers} {}

  vk::Format getFormat() const noexcept { return m_format; }
  vk::Extent3D getExtent() const noexcept { return m_extent; }
  uint32_t getMipLevels() const noexcept { return m_mipLevels; }
  uint32_t getArrayLayers() const noexcept { return m_arrayLayers; }

  vk::Image getHandle() const noexcept {
    ltl::overloader visitor{[](vk::Image img) { return img; },
                            [](const vk::UniqueImage &img) { return *img; }};
    return std::visit(visitor, m_handle);
  }

private:
  std::variant<vk::Image, vk::UniqueImage> m_handle;
  vk::Format m_format;
  vk::Extent3D m_extent;
  uint32_t m_mipLevels;
  uint32_t m_arrayLayers;
};

} // namespace phx
