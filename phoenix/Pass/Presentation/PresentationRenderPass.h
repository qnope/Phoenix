#pragma once

#include <constant.h>
#include <memory>
#include <vkw/Image/SampledImage.h>
#include <vkw/vulkan.h>

namespace phx {

class PhoenixWindow;
template <typename...>
class Framebuffer;

class PresentationRenderPass {
    class Impl;

  public:
    PresentationRenderPass(PhoenixWindow &window) noexcept;

    void setSampledImage(size_t index, SampledImage2dRgbaSrgbRef ref);

    ~PresentationRenderPass();

  private:
    friend vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                                        const PresentationRenderPass &presentationPass) noexcept;

  private:
    std::unique_ptr<Impl> m_impl;
};

} // namespace phx
