#pragma once

#include <SDL2/SDL.h>

#include "VulkanResource.h"
#include "vulkan.h"

namespace phx {
struct ExtentionInvalidException {
    std::vector<std::string> extensions;
};

struct LayerInvalidException {
    std::vector<std::string> layers;
};

class Instance final : public VulkanResource<vk::UniqueInstance> {
  public:
    Instance(SDL_Window *window, bool debug);

    const std::vector<const char *> &getValidationLayers() const noexcept;

  private:
    std::vector<const char *> m_validationLayers;
    vk::DispatchLoaderDynamic m_dispatchLoaderDynamic;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_debugMessenger;
};
} // namespace phx
