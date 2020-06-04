#include "TexturedLambertianMaterial.h"

namespace phx {

TexturedLambertianMaterial::TexturedLambertianMaterial(
    const std::string &path,
    ImageLoader<SampledImage2dRgbaSrgbRef> &imageLoader,
    DescriptorPoolList<TexturedLambertianMaterialSetLayout> &pool) noexcept
    : AbstractMaterial{
          typeid(TexturedLambertianMaterialSetLayout),
          pool
              .allocate({imageLoader.load(
                  path, true, vk::PipelineStageFlagBits::eFragmentShader)})
              .getHandle()} {}

} // namespace phx
