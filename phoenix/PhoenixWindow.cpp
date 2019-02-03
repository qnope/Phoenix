#include "PhoenixWindow.h"

namespace phx {

static auto createWindow(Width w, Height h, WindowTitle windowTitle) {
  SDL_Window *window = SDL_CreateWindow(
      windowTitle.get().data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      static_cast<int>(w.get()), static_cast<int>(h.get()),
      SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    throw PhoenixWindowOpeningException{SDL_GetError()};
  }

  return window;
}

PhoenixWindow::PhoenixWindow(Width width, Height height,
                             WindowTitle windowTitle)
    : m_width{width}, m_height{height}, m_windowHandle{createWindow(
                                            width, height, windowTitle)} {}

bool PhoenixWindow::run() const noexcept {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_WINDOWEVENT) {
      if (event.window.event == SDL_WINDOWEVENT_CLOSE)
        return false;
    }
  }

  return true;
}

Width PhoenixWindow::getWidth() const noexcept { return m_width; }

Height PhoenixWindow::getHeight() const noexcept { return m_height; }

Device &PhoenixWindow::getDevice() noexcept { return m_device; }

} // namespace phx
