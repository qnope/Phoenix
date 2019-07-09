#pragma once

#include "../vulkan.hpp"
#include <ltl/ltl.h>

namespace phx {
namespace output {
struct OutputAttachment {
protected:
  OutputAttachment() = default;
};

struct NormalAttachment : OutputAttachment {
  auto getAttachmentState() {
    vk::PipelineColorBlendAttachmentState state;
    state.blendEnable = false;
    state.colorWriteMask =
        vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eR;
    return state;
  }
};

constexpr NormalAttachment normal_attachment;
} // namespace output

template <typename... Outputs> class WithOutputs {
  static constexpr ltl::type_list_t<Outputs...> output_types{};

public:
  static constexpr auto number_outputs = output_types.length;

  WithOutputs(Outputs... outputs) : m_attachmentStates{outputs.getAttachmentState()...} {
    compileTimeCheck();
  }

  auto getAttachmentStates() { return m_attachmentStates; }

private:
  void compileTimeCheck() {
    using namespace ltl;
    typed_static_assert_msg(
        number_outputs > 0_n,
        "If WithOutputs is specified, it must have at least one output");
    typed_static_assert_msg(
        all_of_type(output_types, is_derived_from(type_v<output::OutputAttachment>)),
        "Outputs must be derived from OutputAttachment");
  }

private:
  std::array<vk::PipelineColorBlendAttachmentState, number_outputs.value>
      m_attachmentStates;
};

LTL_MAKE_IS_KIND(WithOutputs, isWithOutputs);

} // namespace phx
