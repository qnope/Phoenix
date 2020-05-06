#pragma once

#include "vulkan.h"
#include <variant>

#include <ltl/functional.h>

namespace phx {
struct FullBarrier {};

struct MemoryBarrier {
  vk::PipelineStageFlags srcStage;
  vk::PipelineStageFlags dstStage;

  vk::AccessFlags srcMask;
  vk::AccessFlags dstMask;
};

using Barrier = std::variant<FullBarrier, MemoryBarrier>;

namespace details {
void applyBarrier(vk::CommandBuffer cmd, FullBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, MemoryBarrier barrier);
} // namespace details

template <typename T> void applyBarrier(T &&obj, Barrier barrier) {
  std::visit(ltl::curry(lift(details::applyBarrier), FWD(obj)), barrier);
}

} // namespace phx
