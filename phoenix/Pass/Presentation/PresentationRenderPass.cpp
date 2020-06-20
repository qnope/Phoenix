#include "PresentationRenderPass.h"
#include "PresentationSubpass.h"
#include <PhoenixWindow.h>
#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/RenderPassWrapper.h>

#include <vector>

namespace phx {

static auto make_render_pass(PhoenixWindow &phoenixWindow) {
    auto subpass = ltl::tuple_t{buildNoDepthStencilNoInputColorsSubpassDescription(0_n)};
    auto attachment = ltl::tuple_t{phoenixWindow.getAttachmentDescription()};
    auto dependency = ltl::tuple_t{buildPresentationDependency()};

    return phoenixWindow.getDevice().createRenderPass(attachment, subpass, dependency);
}

using Pass = decltype(make_render_pass(std::declval<PhoenixWindow &>()));

class PresentationRenderPass::Impl {
  public:
    Impl(PhoenixWindow &phoenixWindow) noexcept :
        m_phoenixWindow{phoenixWindow},                //
        m_renderPass{make_render_pass(phoenixWindow)}, //
        m_poolManager{phoenixWindow.getDevice()} {
        phoenixWindow.generateFramebuffer(m_renderPass.getHandle());
        m_framebuffers = &phoenixWindow.getFramebuffers();
        m_subpasses.resize(m_framebuffers->size());
    }

    const auto &renderPass() const noexcept { return m_renderPass; }

    const auto &subpass() const noexcept {
        assert(m_subpasses[m_currentIndex]);
        return *m_subpasses[m_currentIndex];
    }

    const auto &framebuffer() const noexcept { return (*m_framebuffers)[m_currentIndex]; }

    void setSampledImage(std::size_t index, SampledImage2dRgbaSrgbRef ref) {
        using Layout = DescriptorSetLayout<SampledImage2dRgbaSrgbBinding>;
        assert(index <= m_framebuffers->size());
        m_currentIndex = index;

        auto &device = m_phoenixWindow.getDevice();
        auto width = m_phoenixWindow.getWidth();
        auto height = m_phoenixWindow.getHeight();
        auto pipeline = make_presentation_pipeline(device, m_renderPass, m_poolManager.layout<Layout>(), width, height);
        auto set = m_poolManager.allocate<Layout>({ref});

        m_subpasses[index] = std::make_unique<PresentationSubpass>(std::move(pipeline), set);
    }

  private:
    PhoenixWindow &m_phoenixWindow;
    Pass m_renderPass;
    DescriptorPoolManager m_poolManager;
    const std::vector<Framebuffer<vk::ImageView>> *m_framebuffers;
    std::vector<std::unique_ptr<PresentationSubpass>> m_subpasses;
    std::size_t m_currentIndex{0};
};

PresentationRenderPass::PresentationRenderPass(PhoenixWindow &window) noexcept :
    m_impl{std::make_unique<Impl>(window)} {}

void PresentationRenderPass::setSampledImage(std::size_t index, SampledImage2dRgbaSrgbRef ref) {
    m_impl->setSampledImage(index, ref);
}

PresentationRenderPass::~PresentationRenderPass() = default;

vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer,
                             const PresentationRenderPass &presentationRenderPass) noexcept {
    auto &impl = *presentationRenderPass.m_impl;
    auto &renderPass = impl.renderPass();
    auto &framebuffer = impl.framebuffer();
    auto &subpass = impl.subpass();

    cmdBuffer << (framebuffer << (renderPass << subpass));
    return cmdBuffer;
}

} // namespace phx
