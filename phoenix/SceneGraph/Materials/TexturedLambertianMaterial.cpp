#include "TexturedLambertianMaterial.h"

namespace phx {

TexturedLambertianMaterial::TexturedLambertianMaterial(
    const std::string &path,
    ImageLoader<SampledImage2dRgbaSrgbRef> &imageLoader,
    DescriptorPoolManager &pool) noexcept
    : AbstractMaterial{
          pool.allocate<TexturedLambertianMaterialSetLayout>({{imageLoader.load(
              path, true, vk::PipelineStageFlagBits::eFragmentShader)}})} {}

} // namespace phx
