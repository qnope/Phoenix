#include "utility.h"

#include <SDL2/SDL_image.h>
#include <ltl/operator.h>

namespace phx {
std::vector<const char *>
to_const_char_vector(const std::vector<std::string> &strings) noexcept {
  return strings | ltl::map(&std::string::c_str);
}

std::vector<std::string>
to_string_vector(std::vector<const char *> &ptrs) noexcept {
  auto to_string = [](const char *p) -> std::string { return p; };
  return ptrs | ltl::map(to_string);
}

std::string readFile(const std::string &path) {
  std::ifstream stream(path, std::ios::binary);

  if (stream) {
    return {std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
  }

  throw FileNotFoundException{path};
}

std::string getBaseDirectory(std::string_view path) noexcept {
  return std::filesystem::path(path).parent_path().string();
}

static auto deleter = [](SDL_Surface *surface) { SDL_FreeSurface(surface); };
using surface_ptr = std::unique_ptr<SDL_Surface, decltype(deleter)>;

static surface_ptr loadSurface(std::string path) {
  surface_ptr img{IMG_Load(path.c_str()), deleter};

  if (!img)
    throw FileNotFoundException{path};

  SDL_PixelFormat format = *img->format;

  if (format.Rmask == 0x000000ff)
    format.format = SDL_PIXELFORMAT_RGBA8888;

  else
    format.format = SDL_PIXELFORMAT_BGRA8888;

  format.BitsPerPixel = 32;
  format.BytesPerPixel = 4;

  return {SDL_ConvertSurface(img.get(), &format, SDL_SWSURFACE), deleter};
}

ltl::tuple_t<Width, Height, std::vector<std::byte>>
loadImage(std::string path) {
  auto img = loadSurface(std::move(path));
  Width width{uint32_t(img->w)};
  Height height{uint32_t(img->h)};
  std::vector<std::byte> values(img->pitch * img->h);
  memcpy(values.data(), img->pixels, values.size());
  return {width, height, std::move(values)};
}

} // namespace phx
