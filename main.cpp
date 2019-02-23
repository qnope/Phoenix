#include <iostream>

#include "phoenix/PhoenixWindow.h"
#include "phoenix/vkw/CommandPool.h"
#include "phoenix/vkw/GraphicPipeline.h"
#include "phoenix/vkw/SubpassBuilder.h"
#include "phoenix/vkw/utility.h"
#include <ltl/ltl.h>

int main(int ac, char **av) {
  using namespace ltl::literals;
  try {
    phx::PhoenixWindow window{phx::Width(800u), phx::Height(600u),
                              phx::WindowTitle("Phoenix Engine")};

    phx::Device &device = window.getDevice();
    vk::Device deviceHandle = device.getHandle();
    auto queue = device.getQueue();

    auto vertexShader = device.createShaderModule<phx::VertexShaderType>(
        "../Phoenix/phoenix/shaders/TriangleTest/triangle.vert", true);

    auto fragmentShader = device.createShaderModule<phx::FragmentShaderType>(
        "../Phoenix/phoenix/shaders/TriangleTest/triangle.frag", true);

    auto pipelineLayout = device.createPipelineLayout();

    auto subpass = phx::buildNoDepthStencilNoInputColors(0_n);
    auto attachment = window.getAttachmentDescription();

    auto renderPass = device.createRenderPass(ltl::tuple_t{attachment},
                                              ltl::tuple_t{subpass}, ltl::tuple_t{});

    window.generateFramebuffer(renderPass.getHandle());

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

    phx::CommandPool pool(deviceHandle, queue.getIndexFamily(), false, false);
    auto commandBuffers = pool.allocateCommandBuffer(vk::CommandBufferLevel::ePrimary,
                                                     window.getImageCount());

    uint32_t i = 0;
    for (vk::CommandBuffer &cb : commandBuffers) {
      vk::CommandBufferBeginInfo bc;
      bc.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
      cb.begin(bc);
      vk::RenderPassBeginInfo br;
      br.renderPass = renderPass.getHandle();
      br.framebuffer = window.getFramebuffer(i++);
      br.renderArea.offset = vk::Offset2D(0, 0);
      br.renderArea.extent =
          vk::Extent2D(window.getWidth().get(), window.getHeight().get());
      vk::ClearValue clearColor{std::array{0.0f, 0.0f, 0.0f, 1.0f}};
      br.clearValueCount = 1;
      br.pClearValues = &clearColor;

      cb.beginRenderPass(br, vk::SubpassContents::eInline);
      cb.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicPipeline.getHandle());
      cb.draw(3, 1, 0, 0);
      cb.endRenderPass();
      cb.end();
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
