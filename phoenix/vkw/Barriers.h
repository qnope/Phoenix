#pragma once

#include "vulkan.h"
#include <variant>

#include <ltl/functional.h>

namespace phx {

struct LayoutTransitionTransferSrcToSampledBarrier {
    vk::Image image;
    vk::PipelineStageFlags dstStage;
    vk::ImageSubresourceRange range;
};

struct LayoutTransitionTransferDstToSampledBarrier {
    vk::Image image;
    vk::PipelineStageFlags dstStage;
    vk::ImageSubresourceRange range;
};

struct LayoutTransitionUndefinedToTransferDstBarrier {
    vk::Image image;
    vk::ImageSubresourceRange range;
};

struct LayoutTransitionTransferDstToSrcBarrier {
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
                             LayoutTransitionTransferDstToSrcBarrier,      //
                             LayoutTransitionTransferDstToSampledBarrier,  //
                             LayoutTransitionTransferSrcToSampledBarrier,  //
                             LayoutTransitionUndefinedToTransferDstBarrier //
                             >;

namespace details {
void applyBarrier(vk::CommandBuffer cmd, FullBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, MemoryBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, LayoutTransitionBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, LayoutTransitionTransferDstToSrcBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, LayoutTransitionTransferSrcToSampledBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, LayoutTransitionTransferDstToSampledBarrier barrier);
void applyBarrier(vk::CommandBuffer cmd, LayoutTransitionUndefinedToTransferDstBarrier barrier);
} // namespace details

template <typename T>
void applyBarrier(T &&obj, Barrier barrier) {
    std::visit(ltl::curry(lift(details::applyBarrier), FWD(obj)), barrier);
}

} // namespace phx
