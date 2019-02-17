#pragma once

#include "With_Pipeline/WithDynamicStates.h"
#include "With_Pipeline/WithOutputs.h"
#include "With_Pipeline/WithScissors.h"
#include "With_Pipeline/WithShaders.h"
#include "With_Pipeline/WithViewports.h"

namespace phx {

template <typename... Args> class GraphicPipeline {
  static constexpr ltl::type_list_t<Args...> types{};

  vk::PrimitiveTopology getTopology() {
    auto hasTopology = ltl::contains_type(types, ltl::type_v<vk::PrimitiveTopology>);
    if_constexpr(hasTopology) {
      auto index = ltl::find_type(types, ltl::type_v<vk::PrimitiveTopology>);
      return m_args[index];
    }
    else return vk::PrimitiveTopology::eTriangleList;
  }

  vk::PolygonMode getPolygonMode() {
    auto hasPolygonMode = ltl::contains_type(types, ltl::type_v<vk::PolygonMode>);
    if_constexpr(hasPolygonMode) {
      auto index = ltl::find_type(types, ltl::type_v<vk::PolygonMode>);
      return m_args[index];
    }
    else return vk::PolygonMode::eFill;
  }

  vk::CullModeFlagBits getCullModeFlag() {
    auto hasCullModeFlag = ltl::contains_type(types, ltl::type_v<vk::CullModeFlagBits>);
    if_constexpr(hasCullModeFlag) {
      auto index = ltl::find_type(types, ltl::type_v<vk::CullModeFlagBits>);
      return m_args[index];
    }
    else return vk::CullModeFlagBits::eNone;
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
    return info;
  }

public:
  static constexpr auto hasDynamicStates = contains_if_type(types, isWithDynamicStates);

  GraphicPipeline(vk::Device device, Args... args) : m_args{std::move(args)...} {
    using namespace ltl;
    compileTimeCheck();

    auto indexShaders = find_if_type(types, isWithShaders);
    auto indexViewports = find_if_type(types, isWithViewports);
    auto indexScissors = find_if_type(types, isWithScissors);
    auto indexOutputs = find_if_type(types, isWithOutputs);

    auto viewports = m_args[indexViewports].getViewports();
    auto scissors = m_args[indexScissors].getScissors();
    vk::GraphicsPipelineCreateInfo info;

    auto stages = m_args[indexShaders].getStages();
    auto inputAssembly = getInputAssembly();

    vk::PipelineViewportStateCreateInfo viewportInfo;
    viewportInfo.viewportCount = static_cast<uint32_t>(viewports.size());
    viewportInfo.scissorCount = static_cast<uint32_t>(scissors.size());
    viewportInfo.pViewports = viewports.data();
    viewportInfo.pScissors = scissors.data();

    auto rasterizationState = getRasterizationState();
    vk::PipelineMultisampleStateCreateInfo multisampleState;

    auto attachmentStates = m_args[indexOutputs].getAttachmentStates();
    vk::PipelineColorBlendStateCreateInfo colorBlendState;
    colorBlendState.attachmentCount = static_cast<uint32_t>(attachmentStates.size());
    colorBlendState.pAttachments = attachmentStates.data();

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
    std::array<vk::DynamicState, 20> dynamicStatesArray;

    if_constexpr(hasDynamicStates) {
      auto indexDynamicStates = find_if_type(types, isWithDynamicStates);
      auto dynamicStates = m_args[indexDynamicStates];
      auto states = dynamicStates.getDynamicStates();

      ltl::copy(states, dynamicStatesArray.begin());

      dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(states.size());
      dynamicStateInfo.pDynamicStates = dynamicStatesArray.data();
      info.pDynamicState = &dynamicStateInfo;
    }

    info.stageCount = static_cast<uint32_t>(stages.size());
    info.pStages = stages.data();
    info.pInputAssemblyState = &inputAssembly;
    info.pViewportState = &viewportInfo;
    info.pRasterizationState = &rasterizationState;
    info.pMultisampleState = &multisampleState;
    info.pDepthStencilState = nullptr;
    info.pColorBlendState = &colorBlendState;
  }

private:
  void compileTimeCheck() {
    using namespace ltl::literals;
    using namespace ltl;

    typed_static_assert_msg(count_if_type(types, isWithShaders) == 1_n,
                            "Graphic Pipelines must have one and only one WithShaders");

    typed_static_assert_msg(count_if_type(types, isWithViewports) == 1_n,
                            "Graphic Pipelines must have one and only one WithViewports");

    typed_static_assert_msg(count_if_type(types, isWithScissors) == 1_n,
                            "Graphic Pipelines must have one and only one WithScissors");

    typed_static_assert_msg(count_if_type(types, isWithOutputs) <= 1_n,
                            "Graphic Pipelines can have only one WithOutputs");

    typed_static_assert_msg(count_if_type(types, isWithDynamicStates) <= 1_n,
                            "Graphic Pipelines can have only one WithDynamicStates");

    typed_static_assert_msg(count_type(types, type_v<vk::PrimitiveTopology>) <= 1_n,
                            "Graphic Pipelines can have only one topology");

    typed_static_assert_msg(count_type(types, type_v<vk::PolygonMode>) <= 1_n,
                            "Graphic Pipelines can have only one polygon mode");

    typed_static_assert_msg(count_type(types, type_v<vk::CullModeFlagBits>) <= 1_n,
                            "Graphic Pipelines can have only one cull mode");

    auto indexViewports = find_if_type(types, isWithViewports);
    auto indexScissors = find_if_type(types, isWithScissors);
    auto viewports = m_args[indexViewports];
    auto scissors = m_args[indexScissors];
    typed_static_assert_msg(viewports.numberViewports == scissors.numberScissors,
                            "The number of viewports must be the same as scissors");

    if_constexpr(hasDynamicStates) {
      auto indexDynamicStates = find_if_type(types, isWithDynamicStates);
      auto dynamicStates = m_args[indexDynamicStates];

      typed_static_assert_msg(m_args[indexViewports].isDynamic ==
                                  dynamicStates.hasDynamicViewport(),
                              "To have dynamic viewport, you must specify the viewport "
                              "dynamic state and use dynamic viewports");

      typed_static_assert_msg(m_args[indexScissors].isDynamic ==
                                  dynamicStates.hasDynamicScissor(),
                              "To have dynamic scissor, you must specify the scissor "
                              "dynamic scissor and use dynamic scissors");
    }

    else {
      typed_static_assert_msg(
          m_args[indexViewports].isStatic,
          "You must specify viewport dynamic state if you want to use dynamic viewport");

      typed_static_assert_msg(
          m_args[indexScissors].isStatic,
          "You must specify scissor dynamic state if you want to use dynamic scissor");
    }
  }

private:
  ltl::tuple_t<Args...> m_args;
};
} // namespace phx
