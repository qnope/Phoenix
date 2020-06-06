#include <iostream>

#include "phoenix/vkw/Descriptor/TemplatedDescriptorPool.h"
#include "phoenix/vkw/MemoryTransfer.h"

#include "phoenix/PhoenixWindow.h"
#include "phoenix/vkw/CommandPool.h"
#include "phoenix/vkw/SubpassBuilder.h"
#include "phoenix/vkw/utility.h"

#include "phoenix/vkw/RenderPassWrapper.h"
#include <ltl/Range/Zip.h>

#include "phoenix/vkw/Vertex.h"

#include "phoenix/vkw/Buffer/BufferList.h"
#include "phoenix/vkw/Descriptor/DescriptorPoolList.h"
#include "phoenix/vkw/Image/ImageLoader.h"
#include "phoenix/vkw/Image/SampledImage.h"

#include "phoenix/SceneGraph/Nodes/ActivableNode.h"
#include "phoenix/SceneGraph/Nodes/GeometryNode.h"
#include "phoenix/SceneGraph/Nodes/Node.h"
#include "phoenix/SceneGraph/SceneGraph.h"
#include "phoenix/SceneGraph/Visitors/ListResultVisitor.h"
#include "phoenix/SceneGraph/Visitors/OneResultVisitor.h"
#include "phoenix/SceneGraph/Visitors/TypedVisitor.h"

#include "phoenix/Pass/GBuffer/GBufferRenderPass.h"

phx::GeometryNode createGeometryNode(phx::SceneGraph &sceneGraph) {
  auto &materialFactory = sceneGraph.materialFactory();
  auto material = materialFactory.createTexturedLambertianMaterial(
      "../resources/images/texture.jpg");

  std::vector<phx::Complete3dVertex> vertices = {
      {{-1.f, -1.f, 0.0f}, {0.0f}, {0.0f}, {0.0f}, {0.0f, 0.0f}},
      {{1.f, -1.f, 0.0f}, {0.0f}, {0.0f}, {0.0f}, {1.0f, 0.0f}},
      {{1.f, 1.f, 0.0f}, {0.0f}, {0.0f}, {0.0f}, {1.0f, 1.0f}},
      {{-1.f, 1.f, 0.0f}, {0.0f}, {0.0f}, {0.0f}, {0.0f, 1.0f}}};

  std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

  auto drawInformations =
      sceneGraph.allocateDrawInformations(vertices, indices);

  return {drawInformations, std::move(material)};
}

int main(int, char **) {
  constexpr auto width = phx::Width{512u};
  constexpr auto height = phx::Height{512u};

  try {
    phx::PhoenixWindow window{width, height,
                              phx::WindowTitle("Phoenix Engine")};
    phx::Device &device = window.getDevice();
    vk::Device deviceHandle = device.getHandle();

    phx::SceneGraph sceneGraph(device);

    auto node = createGeometryNode(sceneGraph);
    sceneGraph.setRootNode(std::move(node));

    sceneGraph.flush(vk::PipelineStageFlagBits::eVertexInput,
                     vk::AccessFlagBits::eIndexRead |
                         vk::AccessFlagBits::eVertexAttributeRead);

    phx::GBufferRenderPass renderPass{
        device, sceneGraph,
        sceneGraph.materialFactory().descriptorPoolManager(), width, height};

    auto queue = device.getQueue();

    // window.generateFramebuffer(renderPass.getHandle());

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

    // for (auto [commandBuffer, framebuffer] :
    //     ltl::zip(commandBuffers, framebuffers)) {
    for (auto commandBuffer : commandBuffers) {
      vk::CommandBufferBeginInfo info;
      commandBuffer.begin(info);

      commandBuffer << renderPass;

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
