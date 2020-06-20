#pragma once

#include "TemplatedGraphicPipeline.h"
#include <ltl/movable_any.h>

namespace phx {

class GraphicPipeline {
  public:
    template <typename T>
    GraphicPipeline(T pipeline) noexcept :
        m_handle{pipeline.getHandle()},                //
        m_pipeline{std::move(pipeline)},               //
        m_pipelineLayout{m_pipeline.get<T>().layout()} //
    {}

    const PipelineLayout &layout() const noexcept { return m_pipelineLayout; }

    vk::Pipeline getHandle() const noexcept { return m_handle; }

  private:
    vk::Pipeline m_handle;
    ltl::movable_any m_pipeline;
    const PipelineLayout &m_pipelineLayout;
};
} // namespace phx
