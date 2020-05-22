#pragma once

#include "vulkan.h"
#include <variant>

#include <ltl/functional.h>

namespace phx {

struct LayoutTransitionTransferToSampledBarrier {
  vk::Image image;
  vk::PipelineStageFlags dstStage;
  vk::ImageSubresourceRange range;
};

struct LayoutTransitionUndefinedToTransferSrcBarrier {
  vk::Image image;
  vk::ImageSubresourceRange range;
};

struct LayoutTransitionBarrier {
  vk::PipelineStageFlags srcStage;
  vk::PipelineStageFlags dstStage;

  vk::AccessFlags srcAccess;
  vk::AccessFlags dstAccess;
  vk::ImageLayout oldLayout;
  vk::ImageLayout newLayout;
  vk::Image image;
  vk::ImageSubresourceRange range;
};

struct FullBarrier {};

struct MemoryBarrier {
  vk::PipelineStageFlags srcStage;
  vk::PipelineStageFlags dstStage;
  vk::AccessFlags srcMask;
  vk::AccessFlags dstMask;
};

using Barrier = std::variant<FullBarrier,                                  //
                             MemoryBarrier,                                //
                             LayoutTransitionBarrier,                      //
                             LayoutTransitionTransferToSampledBarrier,     //
                             LayoutTransitionUndefinedToTransferSrcBarrier //
                             >;

namespace details {
void applyBarrier(vk::CommandBuffer cmd, FullBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, MemoryBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, LayoutTransitionBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd,
                  LayoutTransitionTransferToSampledBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd,
                  LayoutTransitionUndefinedToTransferSrcBarrier barrier);
} // namespace details

template <typename T> void applyBarrier(T &&obj, Barrier barrier) {
  std::visit(ltl::curry(lift(details::applyBarrier), FWD(obj)), barrier);
}

} // namespace phx
