#pragma once
#include "vulkan.hpp"
#include <ltl/ltl.h>

namespace phx {
namespace detail {
template <typename Number> struct SubpassIndex {
  static constexpr Number index{};
  SubpassIndex(Number) {}
  typed_static_assert(ltl::is_number_t(index));
};

LTL_MAKE_IS_KIND(SubpassIndex, isSubpassIndex);

template <typename... As> class CommandBufferWrapperImpl {
  static constexpr ltl::type_list_t<As...> types{};
  static constexpr auto hasRenderPass = ltl::contains_if_type(types, isRenderPass);
  static constexpr auto hasSubpassIndex = ltl::contains_if_type(types, isSubpassIndex);

  constexpr auto getSubpassIndex() const {
    typed_static_assert(hasSubpassIndex);
    constexpr auto subpassIndexIndex = ltl::find_if_type(types, isSubpassIndex);
    using Index = decltype_t(types[subpassIndexIndex]);
    return Index::index;
  }

  constexpr auto getRenderPassType() const {
    typed_static_assert(hasRenderPass);
    constexpr auto indexRenderPass = ltl::find_if_type(types, isRenderPass);
    return types[indexRenderPass];
  }

  constexpr auto getNumberSubpassFromRenderPass() const {
    using RenderPass = decltype_t(getRenderPassType());
    return RenderPass::number_subpasses;
  }

public:
  CommandBufferWrapperImpl(vk::CommandBuffer buffer) : m_commandBuffer{buffer} {}

  template <typename... RP, typename... FB>
  auto withRenderPass(const RenderPass<RP...> &renderPass,
                      const Framebuffer<FB...> &framebuffer) const noexcept {
    typed_static_assert_msg(
        !hasRenderPass,
        "You must not have a RenderPass launched to launch another RenderPass");
    typed_static_assert_msg(
        renderPass.number_attachments == framebuffer.number_attachments,
        "Framebuffer must have the same attachments number as RenderPass attachments.");

    vk::RenderPassBeginInfo info;

    auto clearValues = renderPass.getClearValues();
    info.clearValueCount = static_cast<uint32_t>(clearValues.size());
    info.pClearValues = clearValues.data();
    info.renderArea.offset.x = info.renderArea.offset.y = 0;
    info.renderArea.extent.width = framebuffer.getWidth();
    info.renderArea.extent.height = framebuffer.getHeight();
    info.renderPass = renderPass.getHandle();
    info.framebuffer = framebuffer.getHandle();
    m_commandBuffer.beginRenderPass(info, vk::SubpassContents::eInline);

    return CommandBufferWrapperImpl<RenderPass<RP...>, decltype(SubpassIndex{0_n})>{
        m_commandBuffer};
  }

  template <typename Pass> auto performSubpass(const Pass &pass) const noexcept {
    typed_static_assert_msg(
        hasRenderPass,
        "There is no RenderPass associated or the RenderPass is already ended");
    typed_static_assert_msg(getNumberSubpassFromRenderPass() > getSubpassIndex(),
                            "The RenderPass has already performed all its subpasses");
    pass(m_commandBuffer);

    auto nextSubpassIndex = getSubpassIndex() + 1_n;
    auto isEnd = (nextSubpassIndex == this->getNumberSubpassFromRenderPass());

    if_constexpr(isEnd) {
      m_commandBuffer.endRenderPass();
      return CommandBufferWrapperImpl<>{m_commandBuffer};
    }

    else {
      return CommandBufferWrapperImpl<decltype_t(this->getRenderPassType()),
                                      decltype(SubpassIndex{nextSubpassIndex})>{
          m_commandBuffer};
    }
  }

protected:
  vk::CommandBuffer m_commandBuffer;
};

struct CommandBufferWrapperWithEnding : CommandBufferWrapperImpl<> {
  using CommandBufferWrapperImpl<>::CommandBufferWrapperImpl;
  ~CommandBufferWrapperWithEnding() { m_commandBuffer.end(); }
};

} // namespace detail

class CommandBufferWrapperSimultaneous : public detail::CommandBufferWrapperWithEnding {
public:
  CommandBufferWrapperSimultaneous(vk::CommandBuffer commandBuffer)
      : detail::CommandBufferWrapperWithEnding{commandBuffer} {
    vk::CommandBufferBeginInfo info;
    info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    m_commandBuffer.begin(info);
  }
};
} // namespace phx
