#include <iostream>

#include "phoenix/vkw/Descriptor/DescriptorPool.h"
#include "phoenix/vkw/MemoryTransfer.h"

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

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
};

auto make_render_pass(const phx::PhoenixWindow &window) {
  auto subpass = ltl::tuple_t{phx::buildNoDepthStencilNoInputColors(0_n)};
  auto attachment = ltl::tuple_t{window.getAttachmentDescription()};
  auto dependency = phx::buildPresentationDependency();

  return window.getDevice().createRenderPass(attachment, subpass,
                                             ltl::tuple_t{dependency});
}

auto createDescriptorPool(phx::Device &device) {
  auto binding = phx::DescriptorBinding<VK_SHADER_STAGE_VERTEX_BIT,
                                        vk::DescriptorType::eUniformBuffer, 1,
                                        UniformBufferObject>{};
  auto layout = device.createDescriptorSetLayout(binding);
  return device.createDescriptorPool(std::move(layout));
}

auto create_uniform_buffer(const phx::PhoenixWindow &window) {
  auto uniformBuffer =
      window.getDevice()
          .createBuffer<phx::CpuUniformBuffer<UniformBufferObject>>(1);
  uniformBuffer.setSize(1);

  UniformBufferObject *values = uniformBuffer.ptr();
  values->model = glm::mat4(1.0f);
  values->proj =
      glm::perspective(glm::radians(45.0f), window.getAspect(), 0.1f, 10.0f);
  values->view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f),
                             glm::vec3(0.0f, 0.0f, 1.0f));

  values->proj[1][1] *= -1.0f;

  return uniformBuffer;
}

auto create_buffer_image(phx::Device &device, std::string path) {
  auto [width, height, data] = phx::loadImage(path);
  auto buffer =
      device.createBuffer<phx::StagingBuffer<unsigned char>>(data.size());
  for (auto x : data)
    buffer << x;
  return ltl::tuple_t{width, height, std::move(buffer)};
}

int main([[maybe_unused]] int ac, [[maybe_unused]] char **av) {
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
    auto vertexStagingBuffer =
        device.createBuffer<phx::StagingBuffer<phx::Colored2DVertex>>(4096);

    auto indexStagingBuffer =
        device.createBuffer<phx::StagingBuffer<uint32_t>>(4096);

    for (auto vertex : vertices) {
      vertexStagingBuffer << vertex;
    }

    for (auto index : indices) {
      indexStagingBuffer << index;
    }

    phx::MemoryTransfer memoryTransfer(device);

    auto vertexBuffer =
        device.createBuffer<phx::VertexBuffer<phx::Colored2DVertex>>(4096);
    auto indexBuffer = device.createBuffer<phx::IndexBuffer<uint32_t>>(4096);

    auto barrier = phx::BufferTransferBarrier{
        vk::PipelineStageFlagBits::eVertexInput,
        vk::AccessFlagBits::eIndexRead |
            vk::AccessFlagBits::eVertexAttributeRead};

    memoryTransfer.to(vertexBuffer) << vertexStagingBuffer;
    memoryTransfer.to(indexBuffer) << indexStagingBuffer << barrier;

    auto uniformBuffer = create_uniform_buffer(window);
    auto descriptorPool = createDescriptorPool(device);
    auto descriptorSet = descriptorPool.allocate({uniformBuffer});

    auto queue = device.getQueue();

    auto renderPass = make_render_pass(window);
    auto trianglePass =
        make_triangle_pass(device, width, height, renderPass, vertexBuffer,
                           indexBuffer, descriptorPool, descriptorSet);

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
