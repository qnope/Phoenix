#pragma once

#include "../Device.h"
#include "../MemoryTransfer.h"
#include "BufferInfo.h"
#include <ltl/algos.h>

#include <vector>

namespace phx {
constexpr std::size_t NUMBER_OF_ELEMENTS_BY_BUFFER = 10'000'000;

template <typename BufferInfo>
class BufferList;

template <typename T, VkBufferUsageFlags usages>
class BufferList<BufferInfo<T, usages>> {
    using SrcBuffer = CpuBuffer<T, usages | VK_BUFFER_USAGE_TRANSFER_SRC_BIT>;
    using DstBuffer = GpuBuffer<T, usages | VK_BUFFER_USAGE_TRANSFER_DST_BIT>;

  public:
    BufferList(Device &device) : m_device{device}, m_memoryTransfer{device} {}

    [[nodiscard]] BufferInfo<T, usages> send(const std::vector<T> &values) {
        auto &dstBuffer = getCandidateBuffer(values.size(), m_gpuBuffers);
        auto &srcBuffer = getCandidateBuffer(values.size(), m_stagingBuffer);
        auto offset = dstBuffer.size();
        auto bufferCopy = prepareCopy(values, srcBuffer, dstBuffer);
        m_memoryTransfer.copyBufferRange(srcBuffer, dstBuffer, bufferCopy);
        return {dstBuffer, offset, dstBuffer.size() - offset};
    }

    void flush(vk::PipelineStageFlags dstStage, vk::AccessFlags dstAccess) {
        m_memoryTransfer.applyMemoryBarrier(dstStage, dstAccess);
        m_memoryTransfer.flush();
    }

    void resetOnlyStagingBuffer() { m_stagingBuffer.clear(); }

    void reset() {
        m_stagingBuffer.clear();
        m_gpuBuffers.clear();
    }

  private:
    static vk::BufferCopy prepareCopy(const std::vector<T> &values, SrcBuffer &src, DstBuffer &dst) noexcept {
        vk::BufferCopy range(src.sizeInBytes(), dst.sizeInBytes());
        for (auto v : values)
            src << v;
        range.size = src.sizeInBytes() - range.srcOffset;
        dst.setSize(dst.size() + values.size());
        return range;
    }

    template <typename Buffer>
    auto &getCandidateBuffer(std::size_t size, std::vector<Buffer> &buffers) noexcept {
        auto hasEnoughSpace = [size](const auto &buffer) { return buffer.size() + size < buffer.capacity(); };

        if (auto *buffer = ltl::find_if_ptr(buffers, hasEnoughSpace)) {
            return *buffer;
        }
        return buffers.emplace_back(m_device.createBuffer<Buffer>(NUMBER_OF_ELEMENTS_BY_BUFFER));
    }

  private:
    Device &m_device;
    MemoryTransfer m_memoryTransfer;
    std::vector<SrcBuffer> m_stagingBuffer;
    std::vector<DstBuffer> m_gpuBuffers;
};

} // namespace phx
