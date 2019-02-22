#pragma once

#include "vulkan.hpp"
#include <array>
#include <ltl/ltl.h>
#include <optional>

namespace phx {
template <typename Index> struct AttachmentReference {
  typed_static_assert_msg(ltl::is_number_t(ltl::type_v<Index>), "Index must be a number");
  static constexpr Index index{};
  constexpr AttachmentReference(Index idx, vk::ImageLayout layout)
      : attachmentReference{idx.value, layout} {}

  const vk::AttachmentReference attachmentReference;
};

LTL_MAKE_IS_KIND(AttachmentReference, isAttachmentReference);

template <typename... Args> class Subpass;

template <typename... Outputs, typename... Inputs, typename... DepthStencil,
          typename... Preserve>
class Subpass<ltl::tuple_t<Outputs...>, ltl::tuple_t<Inputs...>,
              ltl::tuple_t<DepthStencil...>, ltl::tuple_t<Preserve...>> {
  static constexpr ltl::type_list_t<Outputs...> output_types{};
  static constexpr ltl::type_list_t<Inputs...> input_types{};
  static constexpr ltl::type_list_t<DepthStencil...> depth_stencil_type{};
  static constexpr ltl::tuple_t<Preserve...> preserve_types{};

  static constexpr auto hasOutputs = output_types.length > ltl::number_v<0>;
  static constexpr auto hasInputs = input_types.length > ltl::number_v<0>;
  static constexpr auto hasPreserve = preserve_types.length > ltl::number_v<0>;
  static constexpr auto hasDepthStencil = depth_stencil_type.length == ltl::number_v<1>;

  static constexpr auto construct() {
    return [](auto... xs) {
      return std::array<vk::AttachmentReference, sizeof...(xs)>{
          xs.attachmentReference...};
    };
  }

public:
  Subpass(ltl::tuple_t<Outputs...> outputs, ltl::tuple_t<Inputs...> inputs,
          ltl::tuple_t<DepthStencil...> depthStencil, ltl::tuple_t<Preserve...>)
      : m_outputs{outputs(construct())}, m_inputs{inputs(construct())},
        m_depthStencil{depthStencil(construct())}, m_preserve{static_cast<uint32_t>(
                                                       Preserve::value)...} {
    using namespace ltl::literals;
    compileTimeCheck();
  }

  auto getSubpassDescription() {
    vk::SubpassDescription description;

    description.colorAttachmentCount = static_cast<uint32_t>(m_outputs.size());
    description.inputAttachmentCount = static_cast<uint32_t>(m_inputs.size());
    description.preserveAttachmentCount = static_cast<uint32_t>(m_preserve.size());
    description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    description.pColorAttachments = m_outputs.data();
    description.pInputAttachments = m_inputs.data();
    description.pPreserveAttachments = m_preserve.data();

    if_constexpr(hasDepthStencil) {
      description.pDepthStencilAttachment = m_depthStencil.data();
    }

    return description;
  }

  static constexpr auto getMaxAttachmentIndex() {
    using namespace ltl::literals;
    constexpr auto maxOutput = ltl::max(-1_n, -1_n, Outputs::index...);
    constexpr auto maxInput = ltl::max(-1_n, -1_n, Inputs::index...);
    constexpr auto maxPreserve = ltl::max(-1_n, -1_n, Preserve{}...);

    return ltl::max(maxOutput, maxInput, maxPreserve);
  }

private:
  void compileTimeCheck() {
    using namespace ltl;
    using namespace ltl::literals;

    typed_static_assert_msg(all_of_type(output_types, isAttachmentReference),
                            "All outputs must be AttachmentReference");

    typed_static_assert_msg(all_of_type(input_types, isAttachmentReference),
                            "All inputs must be AttachmentReference");

    typed_static_assert_msg(
        all_of_type(depth_stencil_type, isAttachmentReference),
        "DepthStencil must be an AttachmentReference if there is one");

    typed_static_assert_msg(is_number_list_t(preserve_types),
                            "All preserve attachments must be number_t");

    typed_static_assert_msg(depth_stencil_type.length <= 1_n,
                            "There can be only zero or one depth stencil attachment");
  }

private:
  std::array<vk::AttachmentReference, sizeof...(Outputs)> m_outputs;
  std::array<vk::AttachmentReference, sizeof...(Inputs)> m_inputs;
  std::array<vk::AttachmentReference, sizeof...(DepthStencil)> m_depthStencil;
  std::array<uint32_t, sizeof...(Preserve)> m_preserve;
};

LTL_MAKE_IS_KIND(Subpass, isSubpass);
} // namespace phx
