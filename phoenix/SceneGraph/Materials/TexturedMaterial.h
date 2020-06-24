#pragma once

#include "AbstractMaterial.h"

#include <vkw/PipelineLayout.h>
#include <vkw/Descriptor/DescriptorSet.h>

namespace phx {

template <typename ManagerType>
class TexturedMaterial : public AbstractMaterial {
  public:
    using Layout = typename ManagerType::Layout;
    static constexpr auto pushConstantRanges = ltl::tuple_t<>{};

    TexturedMaterial(DescriptorSet descriptorSet) noexcept : AbstractMaterial{descriptorSet} {}

    bool isCompatibleWith(const PipelineLayout &pipelineLayout) const noexcept {
        return pipelineLayout.hasLayout(layoutType());
    }

    void bindTo(vk::CommandBuffer cmdBuffer, const PipelineLayout &pipelineLayout) const noexcept {
        pipelineLayout.bind(cmdBuffer, vk::PipelineBindPoint::eGraphics, descriptorSet());
    }

  private:
};
} // namespace phx
