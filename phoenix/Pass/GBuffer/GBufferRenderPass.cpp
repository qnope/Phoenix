#include "GBufferRenderPass.h"
#include <vkw/Device.h>

#include "GBufferOutputSubpass.h"
#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>

#include <vkw/RenderPassWrapper.h>

namespace phx {

static auto buildGBufferSubpassDescription() {
  ltl::tuple_t depth{AttachmentReference{
      0_n, vk::ImageLayout::eDepthStencilAttachmentOptimal}};

  ltl::tuple_t outputs{
      AttachmentReference{1_n, vk::ImageLayout::eColorAttachmentOptimal}};

  return SubpassDescription{outputs, ltl::tuple_t{}, depth, ltl::tuple_t{}};
}

static auto make_render_pass(Device &device) {
  vk::AttachmentDescription attachmentColor{};
  attachmentColor.format = vk::Format::eR8G8B8A8Srgb;
  attachmentColor.samples = vk::SampleCountFlagBits::e1;
  attachmentColor.loadOp = vk::AttachmentLoadOp::eClear;
  attachmentColor.storeOp = vk::AttachmentStoreOp::eStore;
  attachmentColor.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  attachmentColor.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  attachmentColor.initialLayout = vk::ImageLayout::eUndefined;
  attachmentColor.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

  vk::AttachmentDescription attachmentDepth{};
  attachmentDepth.format = vk::Format::eD32Sfloat;
  attachmentDepth.samples = vk::SampleCountFlagBits::e1;
  attachmentDepth.loadOp = vk::AttachmentLoadOp::eClear;
  attachmentDepth.storeOp = vk::AttachmentStoreOp::eStore;
  attachmentDepth.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  attachmentDepth.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  attachmentDepth.initialLayout = vk::ImageLayout::eUndefined;
  attachmentDepth.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

  auto attachments = ltl::tuple_t{attachmentDepth, attachmentColor};
  auto subpasses = ltl::tuple_t{buildGBufferSubpassDescription()};

  return device.createRenderPass(attachments, subpasses, ltl::tuple_t{});
}

using DepthBuffer = Image<vk::ImageType::e2D, vk::Format::eD32Sfloat,
                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                              VK_IMAGE_USAGE_SAMPLED_BIT>;

using DepthBufferView =
    ImageView<vk::ImageViewType::e2D, vk::Format::eD32Sfloat,
              VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                  VK_IMAGE_USAGE_SAMPLED_BIT>;

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

  DepthBuffer m_depthMap = m_device.createImage<DepthBuffer>(
      m_width.get(), m_height.get(), 1u, false, VMA_MEMORY_USAGE_GPU_ONLY);

  DepthBufferView m_depthView =
      m_depthMap.createImageView<vk::ImageViewType::e2D>();

  ColorBuffer m_albedoMap = m_device.createImage<ColorBuffer>(
      m_width.get(), m_height.get(), 1u, false, VMA_MEMORY_USAGE_GPU_ONLY);

  ColorBufferView m_albedoView =
      m_albedoMap.createImageView<vk::ImageViewType::e2D>();

  Sampler m_sampler = m_device.createSampler(vk::Filter::eLinear,
                                             vk::SamplerMipmapMode::eLinear);

  decltype(make_render_pass(m_device)) m_renderPass =
      make_render_pass(m_device);

  Framebuffer<vk::ImageView, vk::ImageView> m_framebuffer =
      m_device.createFramebuffer(m_renderPass.getHandle(), m_width.get(),
                                 m_height.get(), m_depthView.getHandle(),
                                 m_albedoView.getHandle());

  GBufferOutputSubpass m_outputSubpass = make_gbuffer_output_subpass(
      m_device, m_width, m_height, m_descriptorPoolManager, m_renderPass);
};

GBufferRenderPass::GBufferRenderPass(Device &device, SceneGraph &sceneGraph,
                                     Width width, Height height)
    : m_sceneGraph{sceneGraph}, //
      m_impl{std::make_unique<Impl>(
          device,                                               //
          sceneGraph.materialFactory().descriptorPoolManager(), //
          width, height)},
      m_albedoMap{m_impl->getSampledAlbedoMap()} {}

SampledImage2dRgbaSrgbRef GBufferRenderPass::getAlbedoMap() const noexcept {
  return m_impl->getSampledAlbedoMap();
}

GBufferRenderPass::~GBufferRenderPass() = default;

vk::CommandBuffer
operator<<(vk::CommandBuffer cmdBuffer,
           const GBufferRenderPass &gBufferRenderPass) noexcept {
  auto &impl = *gBufferRenderPass.m_impl;
  auto &sceneGraph = gBufferRenderPass.m_sceneGraph;
  auto drawBatches = sceneGraph.dispatch(GetDrawBatchesVisitor{});

  auto &renderPass = impl.renderPass();
  auto &framebuffer = impl.framebuffer();
  auto &outputSubpass = impl.outputSubpass();

  impl.setDrawBatches(drawBatches);

  cmdBuffer << (framebuffer << (renderPass << outputSubpass));

  return cmdBuffer;
}

} // namespace phx
