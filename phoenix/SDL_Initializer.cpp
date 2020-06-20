#include "SDL_Initializer.h"

#include <SDL2/SDL_image.h>

namespace phx {

SDL_Initializer::SDL_Initializer() {
    if (!numberInstance) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
            throw PhoenixSDLInitializationException{SDL_GetError()};

        constexpr auto flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP | IMG_INIT_JPG;

        if (IMG_Init(flags) != flags)
            throw PhoenixSDLInitializationException{IMG_GetError()};
        ++numberInstance;
    }
}

SDL_Initializer::~SDL_Initializer() {
    if (!--numberInstance) {
        IMG_Quit();
        SDL_Quit();
    }
}

} // namespace phx
