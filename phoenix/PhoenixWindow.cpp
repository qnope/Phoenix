#include "PhoenixWindow.h"

namespace phx {

static auto createWindow(Width w, Height h, WindowTitle windowTitle) {
  if (!SDL_WasInit(SDL_INIT_VIDEO))
    SDL_Init(SDL_INIT_VIDEO);
  return SDL_CreateWindow(windowTitle.get().data(), SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED, w.get(), h.get(),
                          SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
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

Width PhoenixWindow::width() const noexcept { return m_width; }

Height PhoenixWindow::height() const noexcept { return m_height; }

PhoenixWindow::~PhoenixWindow() noexcept {
  SDL_DestroyWindow(m_windowHandle);
  SDL_Quit();
}

} // namespace phx
