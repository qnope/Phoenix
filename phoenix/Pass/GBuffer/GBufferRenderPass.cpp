#include "GBufferRenderPass.h"
#include <vkw/Device.h>

#include "../DepthPass/DepthSubpass.h"
#include "GBufferOutputSubpass.h"

#include <vkw/RenderPassWrapper.h>

namespace phx {

static auto buildDepthSubpassDescription() {
    ltl::tuple_t depth{AttachmentReference{0_n, vk::ImageLayout::eDepthStencilAttachmentOptimal}};

    return SubpassDescription{ltl::tuple_t{}, ltl::tuple_t{}, depth, ltl::tuple_t{}};
}

static auto buildGBufferSubpassDescription() {
    ltl::tuple_t depth{AttachmentReference{0_n, vk::ImageLayout::eDepthStencilReadOnlyOptimal}};

    ltl::tuple_t outputs{AttachmentReference{1_n, vk::ImageLayout::eColorAttachmentOptimal}};

    return SubpassDescription{outputs, ltl::tuple_t{}, depth, ltl::tuple_t{}};
}

static auto buildSkySubpassDescription() {
    ltl::tuple_t depth{AttachmentReference{0_n, vk::ImageLayout::eDepthStencilReadOnlyOptimal}};
    ltl::tuple_t outputs{AttachmentReference{2_n, vk::ImageLayout::eColorAttachmentOptimal}};

    return SubpassDescription{outputs, ltl::tuple_t{}, depth, ltl::tuple_t{}};
}

static auto buildDependencies() {
    auto make_depth_dependency = [](auto src, auto dst) {
        return vk::SubpassDependency(
            src, dst, vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
            vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::AccessFlagBits::eDepthStencilAttachmentWrite,
            vk::AccessFlagBits::eDepthStencilAttachmentRead, vk::DependencyFlagBits::eByRegion);
    };

    vk::SubpassDependency depthToGBuffer = make_depth_dependency(0, 1);
    vk::SubpassDependency depthToSky = make_depth_dependency(0, 2);

    vk::SubpassDependency dependencyExternToDepth(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eTopOfPipe,
                                                  vk::PipelineStageFlagBits::eEarlyFragmentTests, vk::AccessFlags(),
                                                  vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    vk::SubpassDependency dependencyExternToColorBuffer(VK_SUBPASS_EXTERNAL, 1, vk::PipelineStageFlagBits::eTopOfPipe,
                                                        vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                        vk::AccessFlags(), vk::AccessFlagBits::eColorAttachmentWrite);

    vk::SubpassDependency dependencyExternToSky(VK_SUBPASS_EXTERNAL, 2, vk::PipelineStageFlagBits::eTopOfPipe,
                                                vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlags(),
                                                vk::AccessFlagBits::eColorAttachmentWrite);

    return ltl::tuple_t{depthToGBuffer, depthToSky, dependencyExternToDepth, dependencyExternToColorBuffer,
                        dependencyExternToSky};
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
    auto subpasses = ltl::tuple_t{buildDepthSubpassDescription(), buildGBufferSubpassDescription()};

    auto dependencies = buildDependencies();
    return device.createRenderPass(attachments, subpasses, dependencies);
}

using DepthBuffer = Image<vk::ImageType::e2D, vk::Format::eD32Sfloat,
                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT>;

using DepthBufferView = ImageView<vk::ImageViewType::e2D, vk::Format::eD32Sfloat,
                                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT>;

using ColorBuffer = Image<vk::ImageType::e2D, vk::Format::eR8G8B8A8Srgb,
                          VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT>;

using ColorBufferView = ImageView<vk::ImageViewType::e2D, vk::Format::eR8G8B8A8Srgb,
                                  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT>;

class GBufferRenderPass::Impl {
  public:
    Impl(Device &device, const MatrixBufferLayout &matrixBufferLayout, //
         Width width, Height height) :
        m_device{device},                         //
        m_matrixBufferLayout{matrixBufferLayout}, //
        m_descriptorPoolManager{device},          //
        m_width{width},                           //
        m_height{height} {}

    auto getSampledAlbedoMap() const noexcept { return SampledImage2dRgbaSrgbRef{m_albedoView, m_sampler}; }

    void setMatrixBufferSetAndDrawBatches(DescriptorSet matrixBufferDescriptorSet,
                                          const std::vector<ltl::tuple_t<DrawBatche, uint32_t>> &drawBatches) noexcept {
        auto toDrawInformationsAndIndex = ltl::unzip([](DrawBatche drawBatch, auto index) {
            return ltl::tuple_t{drawBatch[0_n], index};
        });

        m_depthSubpass.setMatrixBufferAndDrawBatches(matrixBufferDescriptorSet,
                                                     drawBatches | ltl::map(toDrawInformationsAndIndex));
        m_outputSubpass.setMatrixBufferAndDrawBatches(matrixBufferDescriptorSet, drawBatches);
    }

    const auto &renderPass() const noexcept { return m_renderPass; }
    const auto &framebuffer() const noexcept { return m_framebuffer; }
    const auto &depthSubpass() const noexcept { return m_depthSubpass; }
    const auto &outputSubpass() const noexcept { return m_outputSubpass; }

  private:
    Device &m_device;
    const MatrixBufferLayout &m_matrixBufferLayout;
    DescriptorPoolManager m_descriptorPoolManager;

    Width m_width;
    Height m_height;

    DepthBuffer m_depthMap =
        m_device.createImage<DepthBuffer>(m_width.get(), m_height.get(), 1u, false, VMA_MEMORY_USAGE_GPU_ONLY);

    DepthBufferView m_depthView = m_depthMap.createImageView<vk::ImageViewType::e2D>();

    ColorBuffer m_albedoMap =
        m_device.createImage<ColorBuffer>(m_width.get(), m_height.get(), 1u, false, VMA_MEMORY_USAGE_GPU_ONLY);

    ColorBufferView m_albedoView = m_albedoMap.createImageView<vk::ImageViewType::e2D>();

    ColorBuffer m_skyMap =
        m_device.createImage<ColorBuffer>(m_width.get(), m_height.get(), 1u, false, VMA_MEMORY_USAGE_GPU_ONLY);
    ColorBufferView m_skyMapView = m_skyMap.createImageView<vk::ImageViewType::e2D>();

    Sampler m_sampler = m_device.createSampler(vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear);

    decltype(make_render_pass(m_device)) m_renderPass = make_render_pass(m_device);

    Framebuffer<2> m_framebuffer =
        m_device.createFramebuffer(m_renderPass.getHandle(), m_width.get(), m_height.get(), m_depthView, m_albedoView);

    GBufferOutputSubpass m_outputSubpass = make_gbuffer_output_subpass(
        m_device, m_width, m_height, m_descriptorPoolManager, m_renderPass, m_matrixBufferLayout);

    DepthSubpass m_depthSubpass = make_depth_subpass(m_device, m_width, m_height, m_renderPass, m_matrixBufferLayout);
};

GBufferRenderPass::GBufferRenderPass(Device &device, const MatrixBufferLayout &matrixBufferLayout, Width width,
                                     Height height) :
    m_impl{std::make_unique<Impl>(device, matrixBufferLayout, width, height)} {}

void GBufferRenderPass::setBufferDrawBatches(
    DescriptorSet matrixBufferDescriptorSet,
    const std::vector<ltl::tuple_t<DrawBatche, uint32_t>> &drawBatches) noexcept {
    m_impl->setMatrixBufferSetAndDrawBatches(matrixBufferDescriptorSet, drawBatches);
}

SampledImage2dRgbaSrgbRef GBufferRenderPass::getAlbedoMap() const noexcept { return m_impl->getSampledAlbedoMap(); }

GBufferRenderPass::~GBufferRenderPass() = default;

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const GBufferRenderPass &gBufferRenderPass) noexcept {
    auto &impl = *gBufferRenderPass.m_impl;

    auto &renderPass = impl.renderPass();
    auto &framebuffer = impl.framebuffer();
    auto &depthSubpass = impl.depthSubpass();
    auto &outputSubpass = impl.outputSubpass();

    cmdBuffer << (framebuffer << (renderPass << depthSubpass << outputSubpass));

    return cmdBuffer;
}

} // namespace phx
