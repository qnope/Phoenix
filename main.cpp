#include <iostream>

#include "phoenix/PhoenixWindow.h"
#include "phoenix/vkw/CommandPool.h"
#include "phoenix/vkw/GraphicPipeline.h"
#include "phoenix/vkw/SubpassBuilder.h"
#include "phoenix/vkw/utility.h"
#include <lol/lol.h>
#include <ltl/ltl.h>

template <typename Pipeline> class TriangleSubpass {
public:
  TriangleSubpass(Pipeline pipeline) : m_pipeline{std::move(pipeline)} {}

  auto operator()(vk::CommandBuffer cmdBuffer) const noexcept {
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_pipeline.getHandle());
    cmdBuffer.draw(3, 1, 0, 0);
  }

private:
  Pipeline m_pipeline;
};

template <typename... RP>
auto make_triangle_pass(phx::PhoenixWindow &window,
                        const phx::RenderPass<RP...> &renderPass) {
  auto &device = window.getDevice();
  auto vertexShader = device.createShaderModule<phx::VertexShaderType>(
      "../Phoenix/phoenix/shaders/TriangleTest/triangle.vert", true);

  auto fragmentShader = device.createShaderModule<phx::FragmentShaderType>(
      "../Phoenix/phoenix/shaders/TriangleTest/triangle.frag", true);

  auto pipelineLayout = device.createPipelineLayout();

  auto graphicPipeline = device.createGraphicPipeline(
      std::move(pipelineLayout), renderPass, 0_n,
      phx::WithShaders{std::move(vertexShader), std::move(fragmentShader)},
      vk::PrimitiveTopology::eTriangleList,
      phx::WithViewports{
          phx::viewport::StaticViewport{window.getWidth(), window.getHeight()}},
      phx::WithScissors{
          phx::scissor::StaticScissor{window.getWidth(), window.getHeight()}},
      vk::CullModeFlagBits::eNone, vk::PolygonMode::eFill,
      phx::WithOutputs{phx::output::normal_attachment});

  return TriangleSubpass{std::move(graphicPipeline)};
}

auto make_render_pass(const phx::PhoenixWindow &window) {
  using namespace ltl::literals;
  auto subpass = ltl::tuple_t{phx::buildNoDepthStencilNoInputColors(0_n)};
  auto attachment = ltl::tuple_t{window.getAttachmentDescription()};
  return window.getDevice().createRenderPass(attachment, subpass, ltl::tuple_t{});
}

namespace phx {

namespace detail {
template <typename Number> struct SubpassIndex {
  static constexpr Number index{};
  SubpassIndex(Number) {}
  typed_static_assert(ltl::is_number_t(index));
};

LTL_MAKE_IS_KIND(SubpassIndex, isSubpassIndex);

struct EmptyCommandBuffer {};

template <typename... As> class CommandBufferWrapperImpl {
  static constexpr ltl::type_list_t<As...> types{};
  static constexpr auto hasRenderPass = ltl::contains_if_type(types, isRenderPass);
  static constexpr auto hasSubpassIndex = ltl::contains_if_type(types, isSubpassIndex);

  static constexpr auto getSubpassIndex() {
    typed_static_assert(hasSubpassIndex);
    auto indexSubpass = ltl::find_if_type(types, isSubpassIndex);
    using Index = decltype_t(types[indexSubpass]);
    return Index::index;
  }

  static constexpr auto getRenderPassType() {
    typed_static_assert(hasRenderPass);
    auto indexRenderPass = ltl::find_if_type(types, isRenderPass);
    return types[indexRenderPass];
  }

  static constexpr auto getNumberSubpassFromRenderPass() {
    using RP = decltype_t(getRenderPassType());
    return RP::number_subpasses;
  }

public:
  CommandBufferWrapperImpl(vk::CommandBuffer buffer) : m_commandBuffer{buffer} {}

  template <typename... RP, typename... FB>
  auto withRenderPass(const RenderPass<RP...> &renderPass,
                      const Framebuffer<FB...> &framebuffer) const noexcept {
    using namespace ltl::literals;
    typed_static_assert_msg(
        !hasRenderPass,
        "You must not have a RenderPass launched to launch another RenderPass");
    typed_static_assert_msg(
        renderPass.number_attachments == framebuffer.number_attachments,
        "Framebuffer must have the same attachments number as RenderPass attachments.");

    vk::RenderPassBeginInfo info;

    auto clearValues = renderPass.getClearValues();
    info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    info.pClearValues = clearValues.data();
    info.renderArea.offset.x = info.renderArea.offset.y = 0;
    info.renderArea.extent.width = framebuffer.getWidth();
    info.renderArea.extent.height = framebuffer.getHeight();
    info.renderPass = renderPass.getHandle();
    info.framebuffer = framebuffer.getHandle();
    m_commandBuffer.beginRenderPass(info, vk::SubpassContents::eInline);

    return CommandBufferWrapperImpl<RenderPass<RP...>, decltype(SubpassIndex{0_n})>{
        m_commandBuffer};
  }

  template <typename Pass> auto performSubpass(const Pass &pass) const noexcept {
    using namespace ltl::literals;
    typed_static_assert_msg(getNumberSubpassFromRenderPass() > getSubpassIndex(),
                            "The RenderPass has already performed all its subpasses");
    pass(m_commandBuffer);

    constexpr auto nextSubpassIndex = this->getSubpassIndex() + 1_n;
    constexpr auto isEnd = nextSubpassIndex == this->getNumberSubpassFromRenderPass();

    if_constexpr(isEnd) {
      m_commandBuffer.endRenderPass();
      return CommandBufferWrapperImpl<>{m_commandBuffer};
    }

    else {
      return CommandBufferWrapperImpl<decltype_t(this->getRenderPassType()),
                                      decltype(SubpassIndex{nextSubpassIndex})>{
          m_commandBuffer};
    }
  }

protected:
  vk::CommandBuffer m_commandBuffer;
};
} // namespace detail

class CommandBufferWrapperSimultaneous : public detail::CommandBufferWrapperImpl<> {
public:
  CommandBufferWrapperSimultaneous(vk::CommandBuffer commandBuffer)
      : detail::CommandBufferWrapperImpl<>{commandBuffer} {
    vk::CommandBufferBeginInfo info;
    info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    m_commandBuffer.begin(info);
  }

  ~CommandBufferWrapperSimultaneous() { m_commandBuffer.end(); }
};

} // namespace phx

int main(int ac, char **av) {
  using namespace ltl::literals;
  try {
    phx::PhoenixWindow window{phx::Width(800u), phx::Height(600u),
                              phx::WindowTitle("Phoenix Engine")};

    phx::Device &device = window.getDevice();
    vk::Device deviceHandle = device.getHandle();
    auto queue = device.getQueue();

    auto renderPass = make_render_pass(window);
    auto trianglePass = make_triangle_pass(window, renderPass);
    window.generateFramebuffer(renderPass.getHandle());

    phx::CommandPool pool(deviceHandle, queue.getIndexFamily(), false, false);
    auto commandBuffers = pool.allocateCommandBuffer(vk::CommandBufferLevel::ePrimary,
                                                     window.getImageCount());

    uint32_t i = 0;
    for (vk::CommandBuffer cb : commandBuffers) {
      phx::CommandBufferWrapperSimultaneous(cb)
          .withRenderPass(renderPass, window.getFramebuffer(i++))
          .performSubpass(trianglePass);
    }

    vk::UniqueSemaphore renderFinished = device.createSemaphore();

    while (window.run()) {
      window.update();

      vk::SubmitInfo submitInfo;
      vk::PipelineStageFlags stageWait =
          vk::PipelineStageFlagBits::eColorAttachmentOutput;
      auto &rf = *renderFinished;
      auto imgAvailable = window.getImageAvailableSemaphore();
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = &imgAvailable;
      submitInfo.pWaitDstStageMask = &stageWait;

      submitInfo.commandBufferCount = 1;
      uint32_t imgIndex = window.getCurrentImageIndex();
      submitInfo.pCommandBuffers = &commandBuffers[imgIndex];
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = &rf;

      queue.getHandle().submit(1, &submitInfo, vk::Fence());

      vk::PresentInfoKHR pi;
      pi.waitSemaphoreCount = 1;
      pi.pWaitSemaphores = &rf;
      pi.swapchainCount = 1;
      vk::SwapchainKHR swapchain = window.getSwapchainHandle();
      pi.pSwapchains = &swapchain;
      pi.pImageIndices = &imgIndex;
      queue.getHandle().presentKHR(pi);
    }

    deviceHandle.waitIdle();
  }

  catch (phx::ExtentionInvalidException exception) {
    std::cerr << "The extensions ";
    ltl::copy(exception.extensions, std::ostream_iterator<std::string>(std::cerr, ","));
    std::cerr << " are not available" << std::endl;
  }

  catch (phx::LayerInvalidException exception) {
    std::cerr << "The validation layers ";
    ltl::copy(exception.layers, std::ostream_iterator<std::string>(std::cerr, ","));
    std::cerr << " are not available" << std::endl;
  }

  catch (phx::PhoenixSDLInitializationException exception) {
  }

  catch (phx::PhoenixWindowOpeningException exception) {
    std::cerr << "Unable to open the PhoenixWindow : " << exception.exception
              << std::endl;
  }

  catch (phx::NoDeviceCompatibleException) {
    std::cerr << "Unable to find a Vulkan compatible GPU" << std::endl;
  }

  catch (phx::NoGraphicComputeQueueException) {
    std::cerr << "The GPU is not compatible with Graphic or Compute queue" << std::endl;
  }

  catch (phx::UnableToCreateSurfaceException) {
    std::cerr << "Unable to create Surface to draw on" << std::endl;
  }

  catch (phx::FileNotFoundException e) {
    std::cerr << "Unable to open : " << e.path << std::endl;
  }

  catch (phx::ShaderErrorException e) {
    std::cerr << e.error << std::endl;
  }

  return 0;
}
