#include "Surface.h"

namespace phx {
Surface::Surface(SDL_Window *m_window, const Instance &instance) {
  VkSurfaceKHR surface;

  if (!SDL_Vulkan_CreateSurface(m_window, instance.getHandle(), &surface)) {
    throw UnableToCreateSurfaceException{};
  }

  m_handle = vk::UniqueSurfaceKHR(surface, instance.getHandle());
}
} // namespace phx
