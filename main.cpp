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
  return 0;
}
