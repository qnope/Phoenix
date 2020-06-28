#pragma once

#include <ltl/functional.h>

#include <vkw/Descriptor/DescriptorPoolManager.h>
#include <vkw/Device.h>
#include <vkw/GraphicPipeline.h>

#include <SceneGraph/Materials/TexturedLambertianMaterial.h>
#include <SceneGraph/Materials/ColoredLambertianMaterial.h>
#include <SceneGraph/SceneGraph.h>
#include <SceneGraph/Visitors/GetDrawBatchesVisitor.h>

#include "../SceneGraphPass/SceneGraphPass.h"

namespace phx {

class GBufferOutputSubpass : public phx::AbstractSubpass {
  public:
    void addGraphicPipeline(GraphicPipeline pipeline);

    friend vk::CommandBuffer operator<<(vk::CommandBuffer cmdBuffer, const GBufferOutputSubpass &pass) noexcept;

    void setMatrixBufferAndDrawBatches(DescriptorSet matrixBufferDescriptorSet,
                                       std::vector<ltl::tuple_t<DrawBatche, uint32_t>> drawBatches) noexcept;

    GraphicPipeline getCompatiblePipeline(const Material &material) const noexcept;

  private:
    std::vector<GraphicPipeline> m_pipelines;
    std::optional<DescriptorSet> m_descriptorSet;
    std::vector<ltl::tuple_t<DrawBatche, uint32_t>> m_drawBatches;
};

template <typename RenderPass, typename Material>
auto make_gbuffer_output_pipeline(Device &device, Width width, Height height, DescriptorPoolManager &poolManager,
                                  const RenderPass &renderPass, const MatrixBufferLayout &matrixBufferLayout,
                                  ltl::type_t<Material>, const std::string &fragmentPath) {
    auto vertexShader =
        device.createShaderModule<VertexShaderType>("../phoenix/shaders/GBufferPass/GBufferOutput.vert", true);

    auto fragmentShader = device.createShaderModule<FragmentShaderType>(fragmentPath, true);

    const auto &materialLayout = poolManager.layout<typename Material::Layout>();
    auto pipelineLayout =
        device.createPipelineLayout(ltl::tuple_t{PushConstantRange<0, sizeof(uint32_t), VK_SHADER_STAGE_VERTEX_BIT>{}} +
                                        ltl::tuple_t{Material::pushConstantRanges},
                                    ltl::tuple_t{std::cref(matrixBufferLayout), std::cref(materialLayout)});

    auto vertexBinding = phx::Complete3dVertex::getBindingDescription(0_n);

    vk::PipelineDepthStencilStateCreateInfo depthInfo{};
    depthInfo.depthTestEnable = true;
    depthInfo.depthWriteEnable = false;
    depthInfo.depthCompareOp = vk::CompareOp::eEqual;

    return device.createGraphicPipeline(
        std::move(pipelineLayout), renderPass.get(), 1_n, WithBindingDescriptions{vertexBinding},
        WithShaders{std::move(vertexShader), std::move(fragmentShader)},
        WithViewports{viewport::StaticViewport{width, height}}, WithScissors{scissor::StaticScissor{width, height}},
        WithOutputs{output::normal_attachment}, depthInfo);
}

template <typename RenderPass>
auto make_gbuffer_output_subpass(Device &device, Width width, Height height, DescriptorPoolManager &poolManager,
                                 const RenderPass &renderPass, const MatrixBufferLayout &matrixBufferLayout) {

    auto curried = ltl::curry(lift(make_gbuffer_output_pipeline), std::ref(device), width, height,
                              std::ref(poolManager), std::cref(renderPass), std::cref(matrixBufferLayout));

    GBufferOutputSubpass outputSubpass;
    outputSubpass.addGraphicPipeline(GraphicPipeline{curried(
        ltl::type_v<TexturedLambertianMaterial>, "../phoenix/shaders/GBufferPass/GBufferLambertianTexture.frag")});
    outputSubpass.addGraphicPipeline(GraphicPipeline{curried(
        ltl::type_v<ColoredLambertianMaterial>, "../phoenix/shaders/GBufferPass/GBufferLambertianColored.frag")});

    return outputSubpass;
}

} // namespace phx
