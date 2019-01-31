#include "SDL_Initializer.h"

namespace phx {

SDL_Initializer::SDL_Initializer() {
  if (!numberInstance) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
      throw PhoenixSDLInitializationException{SDL_GetError()};
    ++numberInstance;
  }
}

SDL_Initializer::~SDL_Initializer() {
  if (!--numberInstance)
    SDL_Quit();
}

} // namespace phx
