#include "TexturedLambertianMaterial.h"

namespace phx {

TexturedLambertianMaterial::TexturedLambertianMaterial(
    const std::string &path,
    ImageLoader<SampledImage2dRgbaSrgbRef> &imageLoader,
    DescriptorPoolManager &pool) noexcept
    : AbstractMaterial{
          typeid(TexturedLambertianMaterialSetLayout),
          pool
              .allocate<TexturedLambertianMaterialSetLayout>({{imageLoader.load(
                  path, true, vk::PipelineStageFlagBits::eFragmentShader)}})
              .getHandle()} {}

} // namespace phx
