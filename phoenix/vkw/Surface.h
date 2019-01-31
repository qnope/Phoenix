#pragma once
#include <SDL2/SDL_vulkan.h>

#include "Instance.h"
#include "VulkanResource.h"
#include "vulkan.hpp"

namespace phx {
struct UnableToCreateSurfaceException {};
class Surface final : public VulkanResource<vk::UniqueSurfaceKHR> {
public:
  Surface(SDL_Window *m_window, const Instance &instance);

private:
};
} // namespace phx
