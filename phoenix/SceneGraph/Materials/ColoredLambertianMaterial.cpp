#include "ColoredLambertianMaterial.h"

namespace phx {

ColoredLambertianMaterial::ColoredLambertianMaterial(DescriptorSet set, uint32_t index) :
    AbstractMaterial{set}, m_index{index} {}

bool ColoredLambertianMaterial::isCompatibleWith(const PipelineLayout &pipelineLayout) const noexcept {
    std::type_index layoutType = typeid(ColoredLambertianMaterialSetLayout);
    std::type_index pushConstantType = typeid(ColoredLambertianMaterialPushConstant);
    return pipelineLayout.hasLayout(layoutType) && pipelineLayout.hasPushConstant(pushConstantType);
}

void ColoredLambertianMaterial::bindTo(vk::CommandBuffer cmdBuffer, const PipelineLayout &pipelineLayout) const
    noexcept {
    pipelineLayout.bind(cmdBuffer, vk::PipelineBindPoint::eGraphics, descriptorSet());
    pipelineLayout.push<ColoredLambertianMaterialPushConstant>(cmdBuffer, m_index);
}

} // namespace phx
