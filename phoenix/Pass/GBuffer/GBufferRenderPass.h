#pragma once

#include <constant.h>
#include <memory>
#include <vkw/Image/SampledImage.h>

#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>

#include "../SceneGraphPass/SceneGraphPass.h"

namespace phx {
class Device;
struct SkyInformations;

class GBufferRenderPass {
    class Impl;

  public:
    GBufferRenderPass(Device &device, const phx::MatrixBufferLayout &matrixBufferLayout, Width width, Height height);

    void setSkyInformations(const SkyInformations &skyInformations) noexcept;

    void setBufferDrawBatches(DescriptorSet matrixBufferDescriptorSet,
                              const std::vector<ltl::tuple_t<DrawBatche, uint32_t>> &drawBatches) noexcept;

    SampledImage2dRgbaSrgbRef getSkyMap() const noexcept;
    SampledImage2dRgbaSrgbRef getAlbedoMap() const noexcept;

    ~GBufferRenderPass();

  private:
    friend vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                                        const GBufferRenderPass &gBufferRenderPass) noexcept;

  private:
    std::unique_ptr<Impl> m_impl;
};

} // namespace phx
