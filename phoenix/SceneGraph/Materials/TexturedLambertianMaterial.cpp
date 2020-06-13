#include "TexturedLambertianMaterial.h"

namespace phx {

TexturedLambertianMaterial::TexturedLambertianMaterial(
    const std::string &path,
    ImageLoader<SampledImage2dRgbaSrgbRef> &imageLoader,
    DescriptorPoolManager &pool) noexcept
    : AbstractMaterial{
          pool.allocate<TexturedLambertianMaterialSetLayout>({{imageLoader.load(
              path, true, vk::PipelineStageFlagBits::eFragmentShader)}})} {}

void TexturedLambertianMaterial::bindTo(
    vk::CommandBuffer cmdBuffer, const PipelineLayout &pipelineLayout) const
    noexcept {
  auto index = pipelineLayout.descriptorSetIndex(layoutType());
  pipelineLayout.bind(cmdBuffer, vk::PipelineBindPoint::eGraphics, index,
                      descriptorSet());
}

bool TexturedLambertianMaterial::isCompatibleWith(
    const PipelineLayout &pipelineLayout) const noexcept {
  return pipelineLayout.hasLayout(layoutType());
}

} // namespace phx
