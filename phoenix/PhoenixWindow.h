#pragma once

#include <SDL2/SDL.h>
#include <memory>

#include "SDL_Initializer.h"
#include "constant.h"
#include "vkw/Device.h"
#include "vkw/SubpassBuilder.h"
#include "vkw/Surface.h"
#include "vkw/Swapchain.h"

namespace phx {
struct PhoenixWindowOpeningException {
  std::string exception;
};

struct SDL_WindowDeleter {
  void operator()(SDL_Window *window) { SDL_DestroyWindow(window); }
};

class PhoenixWindow {
public:
  PhoenixWindow(Width width, Height height, WindowTitle windowTitle);

  bool run() const noexcept;

  Width getWidth() const noexcept;
  Height getHeight() const noexcept;

  Device &getDevice() noexcept;

  void update() noexcept;

  vk::AttachmentDescription getAttachmentDescription() const noexcept;

  void generateFramebuffer(vk::RenderPass renderPass) noexcept;
  vk::Framebuffer getCurrentFramebuffer() const noexcept;

private:
  SDL_Initializer m_sdl_initializer;
  Width m_width;
  Height m_height;

  std::unique_ptr<SDL_Window, SDL_WindowDeleter> m_windowHandle;
  SDL_Event m_event;
  Instance m_instance{m_windowHandle.get(), true};
  Surface m_surface{m_windowHandle.get(), m_instance};
  Device m_device{m_instance, m_surface};
  Swapchain m_swapchain{m_device, m_surface, m_width, m_height};
};
} // namespace phx
