#include "Barriers.h"
#include "Image/Image.h"

namespace phx {

namespace details {
void applyBarrier(vk::CommandBuffer cmd, FullBarrier) {
  auto fullAccess =
      vk::AccessFlagBits::eMemoryWrite | vk::AccessFlagBits::eMemoryRead;
  cmd.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
                      vk::PipelineStageFlagBits::eAllCommands,
                      vk::DependencyFlags(),
                      vk::MemoryBarrier(fullAccess, fullAccess), {}, {});
}

void applyBarrier(vk::CommandBuffer cmd, MemoryBarrier barrier) {
  vk::MemoryBarrier memoryBarrier(barrier.srcMask, barrier.dstMask);

  cmd.pipelineBarrier(barrier.srcStage, barrier.dstStage, vk::DependencyFlags(),
                      memoryBarrier, {}, {});
}

void applyBarrier(vk::CommandBuffer cmd, LayoutTransitionBarrier barrier) {
  vk::ImageMemoryBarrier memoryBarrier(
      barrier.srcAccess, barrier.dstAccess, barrier.oldLayout,
      barrier.newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
      barrier.image, barrier.range);

  cmd.pipelineBarrier(barrier.srcStage, barrier.dstStage,
                      vk::DependencyFlagBits(), {}, {}, memoryBarrier);
}

void applyBarrier(vk::CommandBuffer cmd,
                  LayoutTransitionTransferDstToSrcBarrier barrier) {
  vk::ImageMemoryBarrier memoryBarrier(
      vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eTransferRead,
      vk::ImageLayout::eTransferDstOptimal,
      vk::ImageLayout::eTransferSrcOptimal, VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED, barrier.image, barrier.range);
  cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                      vk::PipelineStageFlagBits::eTransfer,
                      vk::DependencyFlags(), {}, {}, memoryBarrier);
}

void applyBarrier(vk::CommandBuffer cmd,
                  LayoutTransitionTransferDstToSampledBarrier barrier) {
  vk::ImageMemoryBarrier memoryBarrier(
      vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
      vk::ImageLayout::eTransferDstOptimal,
      vk::ImageLayout::eShaderReadOnlyOptimal, VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED, barrier.image, barrier.range);
  cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, barrier.dstStage,
                      vk::DependencyFlags(), {}, {}, memoryBarrier);
}

void applyBarrier(vk::CommandBuffer cmd,
                  LayoutTransitionTransferSrcToSampledBarrier barrier) {
  vk::ImageMemoryBarrier memoryBarrier(
      vk::AccessFlags(), vk::AccessFlagBits::eShaderRead,
      vk::ImageLayout::eTransferSrcOptimal,
      vk::ImageLayout::eShaderReadOnlyOptimal, VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED, barrier.image, barrier.range);
  cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, barrier.dstStage,
                      vk::DependencyFlags(), {}, {}, memoryBarrier);
}

void applyBarrier(vk::CommandBuffer cmd,
                  LayoutTransitionUndefinedToTransferDstBarrier barrier) {
  vk::ImageMemoryBarrier memoryBarrier(
      vk::AccessFlags(), vk::AccessFlagBits::eTransferWrite,
      vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
      VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, barrier.image,
      barrier.range);
  cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
                      vk::PipelineStageFlagBits::eTransfer,
                      vk::DependencyFlags(), {}, {}, memoryBarrier);
}

} // namespace details

} // namespace phx
