#include <iostream>

#include "ltl/range.h"
#include "phoenix/PhoenixWindow.h"
#include "phoenix/vkw/utility.h"

#include "phoenix/vkw/ShaderCompiler.h"

int main(int ac, char **av) {
  try {
    phx::PhoenixWindow window{phx::Width(800u), phx::Height(600u),
                              phx::WindowTitle("Phoenix Engine")};
    while (window.run()) {
    }
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
