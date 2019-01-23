#pragma once

#include <SDL2/SDL.h>

#include "constant.h"
#include "vkw/Instance.h"

namespace phx {
class PhoenixWindow {
public:
  PhoenixWindow(Width width, Height height, WindowTitle windowTitle);

  bool run() const noexcept;

  Width width() const noexcept;
  Height height() const noexcept;

  void update() noexcept;

  ~PhoenixWindow() noexcept;

private:
  Width m_width;
  Height m_height;

  SDL_Window *m_windowHandle;
  SDL_Event m_event;
  Instance instance{m_windowHandle, true};
};
} // namespace phx
