#pragma once

#include <constant.h>
#include <memory>
#include <vkw/Image/SampledImage.h>

namespace phx {
class Device;
class SceneGraph;
class DescriptorPoolManager;

class GBufferRenderPass {
  class Impl;

public:
  GBufferRenderPass(Device &device, SceneGraph &sceneGraph,
                    DescriptorPoolManager &manager, Width width, Height height);

  SampledImage2dRgbaSrgbRef getAlbedoMap() const noexcept;

  ~GBufferRenderPass();

private:
  friend vk::CommandBuffer
  operator<<(vk::CommandBuffer cmdBuffer,
             const GBufferRenderPass &gBufferRenderPass) noexcept;

private:
  SceneGraph &m_sceneGraph;
  std::unique_ptr<Impl> m_impl;
  SampledImage2dRgbaSrgbRef m_albedoMap;
};

} // namespace phx
