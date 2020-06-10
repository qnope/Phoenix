#pragma once

#include <ltl/tuple_algos.h>

#include "Framebuffer.h"
#include "RenderPass.h"

namespace phx {

template <typename RP, typename... Operations> class RenderPassWrapper;

template <typename... RPs, typename... Operations>
class RenderPassWrapper<RenderPass<RPs...>, Operations...> {
  template <typename T, typename U>
  friend class RenderPassWrapperWithFramebuffer;

  using Pass = RenderPass<RPs...>;
  static constexpr auto all_operations =
      ltl::type_list_v<const Operations &...>;
  static constexpr auto subpass_types =
      ltl::filter_type(all_operations, is_subpass);
  static constexpr auto subpass_number = subpass_types.length;

public:
  static constexpr auto is_complete = Pass::number_subpasses == subpass_number;

  RenderPassWrapper(const RenderPass<RPs...> &renderPass,
                    const Operations &... operations) noexcept
      : renderPass{renderPass}, operations{operations...} {}

  template <typename Operation>
  auto operator<<(const Operation &operation) const noexcept {
    auto buildNextWrapper = [this, &operation](const auto &... operations) {
      return RenderPassWrapper<RenderPass<RPs...>, Operations..., Operation>{
          renderPass, operations..., operation};
    };

    return operations(buildNextWrapper);
  }

  void executeSubpasses(vk::CommandBuffer commandBuffer) const noexcept {
    typed_static_assert(is_complete);

    auto executeOneOperation = [this, commandBuffer](auto operationType) {
      auto index = *ltl::find_type(operations, operationType);
      const auto &operation = operations[index];

      commandBuffer << operation;

      if_constexpr(is_subpass(operationType)) {
        auto subpassIndex = *ltl::find_type(subpass_types, operationType);
        if_constexpr(subpassIndex < subpass_number - 1_n) {
          commandBuffer.nextSubpass(vk::SubpassContents::eInline);
        }
      }
    };

    ltl::for_each(all_operations, executeOneOperation);
  }

public:
  const RenderPass<RPs...> &renderPass;
  ltl::tuple_t<const Operations &...> operations;
};

template <typename Framebuffer, typename RenderPassWrapper>
class RenderPassWrapperWithFramebuffer {
public:
  RenderPassWrapperWithFramebuffer(const Framebuffer &fb,
                                   const RenderPassWrapper &rp) noexcept
      : framebuffer{fb}, renderPassWrapper{rp} {
    typed_static_assert_msg(rp.renderPass.number_attachments ==
                                framebuffer.number_attachments,
                            "Framebuffer must have the same attachments number "
                            "as RenderPass attachments.");
  }

  friend auto operator<<(vk::CommandBuffer commandBuffer,
                         const RenderPassWrapperWithFramebuffer &rp) {
    vk::RenderPassBeginInfo info;

    auto clearValues = rp.renderPassWrapper.renderPass.getClearValues();
    info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    info.pClearValues = clearValues.data();
    info.renderArea.offset.x = info.renderArea.offset.y = 0;
    info.renderArea.extent.width = rp.framebuffer.getWidth();
    info.renderArea.extent.height = rp.framebuffer.getHeight();
    info.renderPass = rp.renderPassWrapper.renderPass.getHandle();
    info.framebuffer = rp.framebuffer.getHandle();

    commandBuffer.beginRenderPass(info, vk::SubpassContents::eInline);

    rp.renderPassWrapper.executeSubpasses(commandBuffer);

    commandBuffer.endRenderPass();

    return commandBuffer;
  }

private:
  const Framebuffer &framebuffer;
  const RenderPassWrapper &renderPassWrapper;
};

template <typename... RPs, typename T>
inline auto operator<<(const RenderPass<RPs...> &renderPass,
                       const T &operation) {
  return RenderPassWrapper<RenderPass<RPs...>, T>{renderPass, operation};
}

template <typename... FBs, typename... RPs>
inline auto operator<<(const Framebuffer<FBs...> &fb,
                       const RenderPassWrapper<RPs...> &wrapper) {
  return RenderPassWrapperWithFramebuffer{fb, wrapper};
}

} // namespace phx
