#pragma once

#include "PipelineLayout.h"
#include "RenderPass/RenderPass.h"
#include "With_Pipeline/WithBuffer.h"
#include "With_Pipeline/WithOutputs.h"
#include "With_Pipeline/WithShaders.h"
#include "With_Pipeline/WithScissors.h"
#include "With_Pipeline/WithViewports.h"
#include "With_Pipeline/WithDynamicStates.h"

namespace phx {

template <typename... Args>
class TemplatedGraphicPipeline;

template <typename... RPs, typename SubpassIndex, typename... Args>
class TemplatedGraphicPipeline<RenderPass<RPs...>, SubpassIndex, Args...> : public VulkanResource<vk::UniquePipeline> {
    static constexpr ltl::type_list_t<Args...> types{};

    vk::PrimitiveTopology getTopology() {
        auto hasTopology = ltl::contains_type(types, ltl::type_v<vk::PrimitiveTopology>);
        if_constexpr(hasTopology) {
            auto index = ltl::find_type(types, ltl::type_v<vk::PrimitiveTopology>);
            return m_args[*index];
        }
        else return vk::PrimitiveTopology::eTriangleList;
    }

    vk::PolygonMode getPolygonMode() {
        auto hasPolygonMode = ltl::contains_type(types, ltl::type_v<vk::PolygonMode>);
        if_constexpr(hasPolygonMode) {
            auto index = ltl::find_type(types, ltl::type_v<vk::PolygonMode>);
            return m_args[*index];
        }
        else return vk::PolygonMode::eFill;
    }

    vk::CullModeFlagBits getCullModeFlag() {
        auto hasCullModeFlag = ltl::contains_type(types, ltl::type_v<vk::CullModeFlagBits>);
        if_constexpr(hasCullModeFlag) {
            auto index = ltl::find_type(types, ltl::type_v<vk::CullModeFlagBits>);
            return m_args[*index];
        }
        else return vk::CullModeFlagBits::eNone;
    }

    vk::PipelineDepthStencilStateCreateInfo *getDepthStencilInfo() {
        auto depthIndex = ltl::find_type(types, ltl::type_v<vk::PipelineDepthStencilStateCreateInfo>);
        if_constexpr(depthIndex.has_value) { //
            return &m_args[*depthIndex];
        }
        else {
            return nullptr;
        }
    }

    auto getInputAssembly() {
        vk::PipelineInputAssemblyStateCreateInfo info;
        info.topology = getTopology();
        return info;
    }

    auto getRasterizationState() {
        vk::PipelineRasterizationStateCreateInfo info;
        info.polygonMode = getPolygonMode();
        info.cullMode = getCullModeFlag();
        info.lineWidth = 1.0f;
        return info;
    }

    vk::PipelineVertexInputStateCreateInfo getVertexInputStateInfo() {
        auto withBindingDescriptionsIndex = ltl::find_if_type(types, is_with_binding_descriptions);
        if_constexpr(withBindingDescriptionsIndex.has_value) {
            const auto &bindingDescription = m_args[*withBindingDescriptionsIndex];
            vk::PipelineVertexInputStateCreateInfo info;
            info.vertexBindingDescriptionCount = uint32_t(bindingDescription.bindings.size());
            info.pVertexBindingDescriptions = bindingDescription.bindings.data();

            info.vertexAttributeDescriptionCount = uint32_t(bindingDescription.attributes.size());
            info.pVertexAttributeDescriptions = bindingDescription.attributes.data();
            return info;
        }
        else {
            return vk::PipelineVertexInputStateCreateInfo{};
        }
    }

  public:
    static constexpr auto hasDynamicStates = contains_if_type(types, is_with_dynamic_states);

    TemplatedGraphicPipeline(vk::Device device, PipelineLayout pipelineLayout, const RenderPass<RPs...> &renderPass,
                             SubpassIndex, Args... args) :
        m_args{std::move(args)...},
        m_pipelineLayout{std::move(pipelineLayout)} {
        using namespace ltl;
        compileTimeCheck();

        auto indexShaders = find_if_type(types, is_with_shaders);
        auto indexViewports = find_if_type(types, is_with_viewports);
        auto indexScissors = find_if_type(types, is_with_scissors);
        auto indexOutputs = find_if_type(types, is_with_outputs);

        auto viewports = m_args[*indexViewports].getViewports();
        auto scissors = m_args[*indexScissors].getScissors();
        vk::GraphicsPipelineCreateInfo info{};

        auto stages = m_args[*indexShaders].getStages();
        auto inputAssembly = getInputAssembly();

        vk::PipelineVertexInputStateCreateInfo inputState = getVertexInputStateInfo();

        vk::PipelineViewportStateCreateInfo viewportInfo;
        viewportInfo.viewportCount = static_cast<uint32_t>(viewports.size());
        viewportInfo.scissorCount = static_cast<uint32_t>(scissors.size());
        viewportInfo.pViewports = viewports.data();
        viewportInfo.pScissors = scissors.data();

        auto rasterizationState = getRasterizationState();
        vk::PipelineMultisampleStateCreateInfo multisampleState;

        vk::PipelineColorBlendStateCreateInfo colorBlendState;

        if_constexpr(indexOutputs.has_value) {
            auto attachmentStates = m_args[*indexOutputs].getAttachmentStates();
            colorBlendState.attachmentCount = static_cast<uint32_t>(attachmentStates.size());
            colorBlendState.pAttachments = attachmentStates.data();
        }
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
        std::array<vk::DynamicState, 20> dynamicStatesArray;
        (void)dynamicStatesArray;

        if_constexpr(hasDynamicStates) {
            auto indexDynamicStates = find_if_type(types, is_with_dynamic_states);
            auto dynamicStates = m_args[indexDynamicStates];
            auto states = dynamicStates.getDynamicStates();

            ltl::copy(states, dynamicStatesArray.begin());

            dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(states.size());
            dynamicStateInfo.pDynamicStates = dynamicStatesArray.data();
            info.pDynamicState = &dynamicStateInfo;
        }

        info.stageCount = static_cast<uint32_t>(stages.size());
        info.pStages = stages.data();
        info.pVertexInputState = &inputState;
        info.pInputAssemblyState = &inputAssembly;
        info.pViewportState = &viewportInfo;
        info.pRasterizationState = &rasterizationState;
        info.pMultisampleState = &multisampleState;
        info.pDepthStencilState = getDepthStencilInfo();
        info.pColorBlendState = &colorBlendState;
        info.layout = m_pipelineLayout.getHandle();
        info.renderPass = renderPass.getHandle();
        info.subpass = SubpassIndex::value;
        m_handle = device.createGraphicsPipelineUnique(vk::PipelineCache(), info);
    }

    const auto &layout() const noexcept { return m_pipelineLayout; }

  private:
    void compileTimeCheck() {
        using namespace ltl;

        typed_static_assert_msg(count_if_type(types, is_with_shaders) == 1_n,
                                "Graphic Pipelines must have one and only one WithShaders");

        typed_static_assert_msg(count_if_type(types, is_with_viewports) == 1_n,
                                "Graphic Pipelines must have one and only one WithViewports");

        typed_static_assert_msg(count_if_type(types, is_with_scissors) == 1_n,
                                "Graphic Pipelines must have one and only one WithScissors");

        typed_static_assert_msg(count_if_type(types, is_with_outputs) <= 1_n,
                                "Graphic Pipelines can have only one WithOutputs");

        typed_static_assert_msg(count_if_type(types, is_with_dynamic_states) <= 1_n,
                                "Graphic Pipelines can have only one WithDynamicStates");

        typed_static_assert_msg(count_type(types, type_v<vk::PrimitiveTopology>) <= 1_n,
                                "Graphic Pipelines can have only one topology");

        typed_static_assert_msg(count_type(types, type_v<vk::PolygonMode>) <= 1_n,
                                "Graphic Pipelines can have only one polygon mode");

        typed_static_assert_msg(count_type(types, type_v<vk::CullModeFlagBits>) <= 1_n,
                                "Graphic Pipelines can have only one cull mode");

        auto indexViewports = find_if_type(types, is_with_viewports);
        auto indexScissors = find_if_type(types, is_with_scissors);
        auto viewports = m_args[*indexViewports];
        auto scissors = m_args[*indexScissors];
        typed_static_assert_msg(viewports.numberViewports == scissors.numberScissors,
                                "The number of viewports must be the same as scissors");

        if_constexpr(hasDynamicStates) {
            auto indexDynamicStates = find_if_type(types, is_with_dynamic_states);
            auto dynamicStates = m_args[indexDynamicStates];

            typed_static_assert_msg(m_args[indexViewports].isDynamic == dynamicStates.hasDynamicViewport(),
                                    "To have dynamic viewport, you must specify the viewport "
                                    "dynamic state and use dynamic viewports");

            typed_static_assert_msg(m_args[indexScissors].isDynamic == dynamicStates.hasDynamicScissor(),
                                    "To have dynamic scissor, you must specify the scissor "
                                    "dynamic scissor and use dynamic scissors");
        }

        else {
            typed_static_assert_msg(m_args[*indexViewports].isStatic, "You must specify viewport dynamic state if you "
                                                                      "want to use dynamic viewport");

            typed_static_assert_msg(m_args[*indexScissors].isStatic, "You must specify scissor dynamic state if you "
                                                                     "want to use dynamic scissor");
        }

        typed_static_assert(ltl::is_number_t(SubpassIndex{}));
        typed_static_assert_msg(RenderPass<RPs...>::number_subpasses > SubpassIndex{},
                                "The Subpass Index must be less than the number of subpass "
                                "within the RenderPass");
    }

  private:
    ltl::tuple_t<Args...> m_args;
    PipelineLayout m_pipelineLayout;
};

LTL_MAKE_IS_KIND(TemplatedGraphicPipeline, is_graphic_pipeline, IsGraphicPipeline, typename);
} // namespace phx
