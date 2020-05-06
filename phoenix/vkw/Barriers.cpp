#include "Barriers.h"

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
} // namespace details

} // namespace phx
