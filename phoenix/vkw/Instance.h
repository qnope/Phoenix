#pragma once

#include <SDL2/SDL.h>

#include "vulkan.hpp"

namespace phx {
struct ExtentionInvalidException {
  std::string extension;
};

struct LayerInvalidException {
  std::string layer;
};

class Instance final {
public:
  Instance(SDL_Window *window, bool debug);

  std::vector<vk::PhysicalDevice> physicalDevices() const noexcept;

private:
  vk::UniqueInstance m_instance;
  vk::DispatchLoaderDynamic m_dispatchLoaderDynamic;
  vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>
      m_debugMessenger;
};
} // namespace phx
