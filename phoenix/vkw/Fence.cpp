#include "Fence.h"

namespace phx {

Fence::Fence(vk::Device device, bool signaledState) noexcept : m_device{device} {
    vk::FenceCreateInfo info;
    if (signaledState)
        info.flags = vk::FenceCreateFlagBits::eSignaled;
    m_handle = device.createFenceUnique(info);
}

void Fence::reset() noexcept { m_device.resetFences({*m_handle}); }

void Fence::wait(uint64_t timeout) noexcept { m_device.waitForFences({*m_handle}, true, timeout); }

void Fence::waitAndReset() noexcept {
    wait();
    reset();
}

} // namespace phx
