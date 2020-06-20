#pragma once

#include <constant.h>
#include <memory>
#include <vkw/Image/SampledImage.h>

#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>

#include "../SceneGraphPass/SceneGraphPass.h"

namespace phx {
class Device;

class GBufferRenderPass {
    class Impl;

  public:
    GBufferRenderPass(Device &device, const phx::MatrixBufferLayout &matrixBufferLayout, Width width, Height height);

    void setBufferDrawBatches(DescriptorSet matrixBufferDescriptorSet,
                              const std::vector<DrawBatche> &drawBatches) noexcept;

    SampledImage2dRgbaSrgbRef getAlbedoMap() const noexcept;

    ~GBufferRenderPass();

  private:
    friend vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                                        const GBufferRenderPass &gBufferRenderPass) noexcept;

  private:
    std::unique_ptr<Impl> m_impl;
};

} // namespace phx
