#pragma once

#include <vkw/Descriptor/DescriptorSetLayout.h>
#include <vkw/PipelineLayout.h>
#include "AbstractMaterial.h"

namespace phx {
class BufferManager;

using ColoredLambertianMaterialSetLayout = DescriptorSetLayout<
    DescriptorBinding<VK_SHADER_STAGE_FRAGMENT_BIT, vk::DescriptorType::eStorageBuffer, 1, glm::vec4>>;

using ColoredLambertianMaterialPushConstant = PushConstantRange<4, 4, VK_SHADER_STAGE_FRAGMENT_BIT>;

class ColoredLambertianMaterial : public AbstractMaterial {
  public:
    ColoredLambertianMaterial(DescriptorSet set, uint32_t index);

    bool isCompatibleWith(const PipelineLayout &pipelineLayout) const noexcept;

    void bindTo(vk::CommandBuffer cmdBuffer, const PipelineLayout &pipelineLayout) const noexcept;

  private:
    uint32_t m_index;
};

} // namespace phx
