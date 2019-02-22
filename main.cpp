#include <iostream>

#include "phoenix/PhoenixWindow.h"
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

    auto vertexShader = device.createShaderModule<phx::VertexShaderType>(
        "../Phoenix/phoenix/shaders/TriangleTest/triangle.vert", true);

    auto fragmentShader = device.createShaderModule<phx::FragmentShaderType>(
        "../Phoenix/phoenix/shaders/TriangleTest/triangle.frag", true);

    auto pipelineLayout = device.createPipelineLayout();

    auto subpass = phx::buildNoDepthStencilNoInputColors(0_n);
    auto attachment = window.getAttachmentDescription();

    auto renderPass = device.createRenderPass(ltl::tuple_t{attachment},
                                              ltl::tuple_t{subpass}, ltl::tuple_t{});

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

    while (window.run()) {
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
