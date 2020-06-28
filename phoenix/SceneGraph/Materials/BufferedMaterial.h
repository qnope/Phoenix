#pragma once

#include <vkw/PipelineLayout.h>
#include "AbstractMaterial.h"

namespace phx {
template <typename ManagerType>
class BufferedMaterial : public AbstractMaterial {
  public:
    using Layout = typename ManagerType::Layout;
    static constexpr auto pushConstantRanges = PushConstantRange<4, 4, VK_SHADER_STAGE_FRAGMENT_BIT>{};

    BufferedMaterial(DescriptorSet set, uint32_t index) : AbstractMaterial{set}, m_index{index} {}

    bool isCompatibleWith(const PipelineLayout &pipelineLayout) const noexcept {
        std::type_index pushConstantType = typeid(PushConstantRange<4, 4, VK_SHADER_STAGE_FRAGMENT_BIT>);
        return pipelineLayout.hasLayout(descriptorSet().layoutType()) &&
               pipelineLayout.hasPushConstant(pushConstantType);
    }

    void bindTo(CommandBufferWrapper &cmdBuffer, const PipelineLayout &pipelineLayout) const noexcept {
        pipelineLayout.bind(cmdBuffer, vk::PipelineBindPoint::eGraphics, descriptorSet());
        pipelineLayout.push<decltype(pushConstantRanges)>(cmdBuffer, m_index);
    }

  private:
    uint32_t m_index;
};
} // namespace phx
