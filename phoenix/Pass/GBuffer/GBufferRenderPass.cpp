#include "GBufferRenderPass.h"
#include <vkw/Device.h>
#include <vkw/SubpassBuilder.h>

#include "GBufferOutputSubpass.h"
#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>

#include <vkw/RenderPassWrapper.h>

namespace phx {

static auto make_render_pass(Device &device) {
  vk::AttachmentDescription attachment{};
  attachment.format = vk::Format::eR8G8B8A8Srgb;
  attachment.samples = vk::SampleCountFlagBits::e1;
  attachment.loadOp = vk::AttachmentLoadOp::eClear;
  attachment.storeOp = vk::AttachmentStoreOp::eStore;
  attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  attachment.initialLayout = vk::ImageLayout::eUndefined;
  attachment.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

  auto subpass = ltl::tuple_t{phx::buildNoDepthStencilNoInputColors(0_n)};
  auto dependency = phx::buildPresentationDependency();

  return device.createRenderPass(ltl::tuple_t{attachment}, subpass,
                                 ltl::tuple_t{dependency});
}

using ColorBuffer =
    Image<vk::ImageType::e2D, vk::Format::eR8G8B8A8Srgb,
          VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT>;

using ColorBufferView =
    ImageView<vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Srgb,
              VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT>;

class GBufferRenderPass::Impl {
public:
  Impl(Device &device, DescriptorPoolManager &poolManager, Width width,
       Height height)
      : m_device{device},                     //
        m_descriptorPoolManager{poolManager}, //
        m_width{width},                       //
        m_height{height} {}

  auto getSampledAlbedoMap() const noexcept {
    return SampledImage2dRgbaSrgbRef{m_albedoView, m_sampler};
  }

  void setDrawBatches(const std::vector<DrawBatche> &drawBatches) noexcept {
    m_outputSubpass.setDrawBatches(&drawBatches);
  }

  const auto &renderPass() const noexcept { return m_renderPass; }
  const auto &framebuffer() const noexcept { return m_framebuffer; }
  const auto &outputSubpass() const noexcept { return m_outputSubpass; }

private:
  Device &m_device;
  DescriptorPoolManager &m_descriptorPoolManager;

  Width m_width;
  Height m_height;

  ColorBuffer m_albedoMap = m_device.createImage<ColorBuffer>(
      m_width.get(), m_height.get(), 1u, false, VMA_MEMORY_USAGE_GPU_ONLY);

  ColorBufferView m_albedoView =
      m_albedoMap.createImageView<vk::ImageViewType::e2D>();

  Sampler m_sampler = m_device.createSampler(vk::Filter::eLinear,
                                             vk::SamplerMipmapMode::eLinear);

  decltype(make_render_pass(m_device)) m_renderPass =
      make_render_pass(m_device);

  Framebuffer<vk::ImageView> m_framebuffer =
      m_device.createFramebuffer(m_renderPass.getHandle(), m_width.get(),
                                 m_height.get(), m_albedoView.getHandle());

  GBufferOutputSubpass m_outputSubpass = make_gbuffer_output_subpass(
      m_device, m_width, m_height, m_descriptorPoolManager, m_renderPass);
};

GBufferRenderPass::GBufferRenderPass(Device &device, SceneGraph &sceneGraph,
                                     DescriptorPoolManager &manager,
                                     Width width, Height height)
    : m_sceneGraph{sceneGraph}, //
      m_impl{std::make_unique<Impl>(device, manager, width, height)},
      m_albedoMap{m_impl->getSampledAlbedoMap()} {}

GBufferRenderPass::~GBufferRenderPass() = default;

vk::CommandBuffer
operator<<(vk::CommandBuffer cmdBuffer,
           const GBufferRenderPass &gBufferRenderPass) noexcept {
  auto &impl = *gBufferRenderPass.m_impl;
  auto &sceneGraph = gBufferRenderPass.m_sceneGraph;
  auto drawBatches = sceneGraph.dispatch(makeGetDrawBatchesVisitor());

  auto &renderPass = impl.renderPass();
  auto &framebuffer = impl.framebuffer();
  auto &outputSubpass = impl.outputSubpass();

  impl.setDrawBatches(drawBatches);

  cmdBuffer << (framebuffer << (renderPass << outputSubpass));

  return cmdBuffer;
}

} // namespace phx
