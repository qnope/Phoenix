#include <iostream>

#include "phoenix/PhoenixWindow.h"

int main(int ac, char **av) {
  try {
    phx::PhoenixWindow window{phx::Width(800u), phx::Height(600u),
                              phx::WindowTitle("Phoenix Engine")};
    while (window.run()) {
    }
  }

  catch (phx::ExtentionInvalidException exception) {
    std::cerr << "The extension " << exception.extension << " is not available"
              << std::endl;
  }

  catch (phx::LayerInvalidException exception) {
    std::cerr << "The validation layer " << exception.layer
              << " is not available" << std::endl;
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

  return 0;
}
