#include <iostream>

#include "ex/triangle.h"

#include "phoenix/PhoenixWindow.h"
#include "phoenix/vkw/CommandPool.h"
#include "phoenix/vkw/GraphicPipeline.h"
#include "phoenix/vkw/SubpassBuilder.h"
#include "phoenix/vkw/utility.h"

#include "phoenix/vkw/RenderPassWrapper.h"
#include <ltl/Range/Zip.h>

#include "phoenix/vkw/Vertex.h"

#include "phoenix/vkw/Buffer/Buffer.h"

auto make_render_pass(const phx::PhoenixWindow &window) {
  auto subpass = ltl::tuple_t{phx::buildNoDepthStencilNoInputColors(0_n)};
  auto attachment = ltl::tuple_t{window.getAttachmentDescription()};
  auto dependency = phx::buildPresentationDependency();

  return window.getDevice().createRenderPass(attachment, subpass,
                                             ltl::tuple_t{dependency});
}

int main(int ac, char **av) {
  constexpr auto width = phx::Width{800u};
  constexpr auto height = phx::Height{600u};

  std::vector<phx::Colored2DVertex> vertices = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

  std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

  try {
    phx::PhoenixWindow window{width, height,
                              phx::WindowTitle("Phoenix Engine")};
    phx::Device &device = window.getDevice();
    vk::Device deviceHandle = device.getHandle();
    auto vertexBuffer =
        device.createBuffer<phx::CpuVertexBuffer<phx::Colored2DVertex>>(4096);

    auto indexBuffer = device.createBuffer<phx::CpuIndexBuffer<uint32_t>>(4096);

    for (auto vertex : vertices) {
      vertexBuffer << vertex;
    }

    for (auto index : indices) {
      indexBuffer << index;
    }

    auto queue = device.getQueue();
    auto renderPass = make_render_pass(window);
    auto trianglePass = make_triangle_pass(device, width, height, renderPass,
                                           vertexBuffer, indexBuffer);

    window.generateFramebuffer(renderPass.getHandle());

    phx::CommandPool pool(deviceHandle, queue.getIndexFamily(), false, false);
    auto commandBuffers = pool.allocateCommandBuffer(
        vk::CommandBufferLevel::ePrimary, window.getImageCount());

    auto &framebuffers = window.getFramebuffers();

    vk::UniqueSemaphore renderFinished = device.createSemaphore();

    auto imgAvailable =
        phx::WaitSemaphore{window.getImageAvailableSemaphore(),
                           vk::PipelineStageFlagBits::eColorAttachmentOutput};

    std::vector<phx::Fence> fences;

    for (auto i = 0u; i < window.getImageCount(); ++i)
      fences.emplace_back(device.createFence(true));

    for (auto [commandBuffer, framebuffer] :
         ltl::zip(commandBuffers, framebuffers)) {
      vk::CommandBufferBeginInfo info;
      commandBuffer.begin(info);

      commandBuffer << (framebuffer << (renderPass << trianglePass));

      commandBuffer.end();
    }

    while (window.run()) {
      window.update();

      uint32_t imgIndex = window.getCurrentImageIndex();

      fences[imgIndex].waitAndReset();

      queue << imgAvailable << commandBuffers[imgIndex] << *renderFinished
            << phx::flush(fences[imgIndex]);

      queue.present(*renderFinished, window.getSwapchainHandle(), imgIndex);
    }
    ltl::for_each(fences, &phx::Fence::waitAndReset);
  }

  catch (phx::ExtentionInvalidException exception) {
    std::cerr << "The extensions ";
    ltl::copy(exception.extensions,
              std::ostream_iterator<std::string>(std::cerr, ","));
    std::cerr << " are not available" << std::endl;
  }

  catch (phx::LayerInvalidException exception) {
    std::cerr << "The validation layers ";
    ltl::copy(exception.layers,
              std::ostream_iterator<std::string>(std::cerr, ","));
    std::cerr << " are not available" << std::endl;
  }

  catch (phx::PhoenixSDLInitializationException exception) {
    std::cerr << "Unable to open the Phoenix : " << exception.exception
              << std::endl;
  }

  catch (phx::PhoenixWindowOpeningException exception) {
    std::cerr << "Unable to open the PhoenixWindow : " << exception.exception
              << std::endl;
  }

  catch (phx::NoDeviceCompatibleException) {
    std::cerr << "Unable to find a Vulkan compatible GPU" << std::endl;
  }

  catch (phx::NoGraphicComputeQueueException) {
    std::cerr << "The GPU is not compatible with Graphic or Compute queue"
              << std::endl;
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
