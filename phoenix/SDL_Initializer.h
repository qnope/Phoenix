#pragma once

#include <SDL2/SDL.h>
#include <string>

namespace phx {
struct PhoenixSDLInitializationException {
  std::string exception;
};

class SDL_Initializer {
public:
  SDL_Initializer();
  ~SDL_Initializer();

private:
  inline static std::size_t numberInstance = 0;
};

} // namespace phx
