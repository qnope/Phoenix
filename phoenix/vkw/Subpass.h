#pragma once

#include "vulkan.h"
#include <array>
#include <ltl/traits.h>
#include <ltl/tuple_algos.h>
#include <optional>

namespace phx {
template <typename Index> struct AttachmentReference {
  typed_static_assert_msg(ltl::is_number_t(ltl::type_v<Index>),
                          "Index must be a number");
  static constexpr Index index{};
  constexpr AttachmentReference(Index idx, vk::ImageLayout layout)
      : attachmentReference{idx.value, layout} {}

  const vk::AttachmentReference attachmentReference;
};

LTL_MAKE_IS_KIND(AttachmentReference, is_attachment_reference,
                 IsAttachmentReference, typename);

template <typename... Args> class Subpass;

template <typename... Outputs, typename... Inputs, typename... DepthStencil,
          typename... Preserve>
class Subpass<ltl::tuple_t<Outputs...>, ltl::tuple_t<Inputs...>,
              ltl::tuple_t<DepthStencil...>, ltl::tuple_t<Preserve...>> {
  static constexpr ltl::type_list_t<Outputs...> output_types{};
  static constexpr ltl::type_list_t<Inputs...> input_types{};
  static constexpr ltl::type_list_t<DepthStencil...> depth_stencil_type{};
  static constexpr ltl::tuple_t<Preserve...> preserve_types{};

  static constexpr auto hasOutputs = !output_types.isEmpty;
  static constexpr auto hasInputs = !input_types.isEmpty;
  static constexpr auto hasPreserve = !preserve_types.isEmpty;
  static constexpr auto hasDepthStencil = !depth_stencil_type.isEmpty;

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
        m_depthStencil{depthStencil(construct())},
        m_preserve{static_cast<uint32_t>(Preserve::value)...} {
    compileTimeCheck();
  }

  auto getSubpassDescription() const {
    vk::SubpassDescription description;

    description.colorAttachmentCount = static_cast<uint32_t>(m_outputs.size());
    description.inputAttachmentCount = static_cast<uint32_t>(m_inputs.size());
    description.preserveAttachmentCount =
        static_cast<uint32_t>(m_preserve.size());
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
    constexpr auto minimum = number_v<-1>;
    constexpr auto maxOutput = ltl::max(minimum, Outputs::index...);
    constexpr auto maxInput = ltl::max(minimum, Inputs::index...);
    constexpr auto maxPreserve = ltl::max(minimum, Preserve{}...);

    return ltl::max(maxOutput, maxInput, maxPreserve);
  }

private:
  constexpr void compileTimeCheck() {
    using namespace ltl;

    typed_static_assert_msg(all_of_type(output_types, is_attachment_reference),
                            "All outputs must be AttachmentReference");

    typed_static_assert_msg(all_of_type(input_types, is_attachment_reference),
                            "All inputs must be AttachmentReference");

    typed_static_assert_msg(
        all_of_type(depth_stencil_type, is_attachment_reference),
        "DepthStencil must be an AttachmentReference if there is one");

    typed_static_assert_msg(is_number_list_t(preserve_types),
                            "All preserve attachments must be number_t");

    typed_static_assert_msg(
        depth_stencil_type.length <= 1_n,
        "There can be only zero or one depth stencil attachment");
  }

private:
  std::array<vk::AttachmentReference, sizeof...(Outputs)> m_outputs;
  std::array<vk::AttachmentReference, sizeof...(Inputs)> m_inputs;
  std::array<vk::AttachmentReference, sizeof...(DepthStencil)> m_depthStencil;
  std::array<uint32_t, sizeof...(Preserve)> m_preserve;
};

template <typename... Ts> Subpass(Ts...)->Subpass<Ts...>;

LTL_MAKE_IS_KIND(Subpass, is_subpass, IsSubpass, typename);
} // namespace phx
