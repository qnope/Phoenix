#pragma once

#include <ltl/movable_any.h>
#include "BufferList.h"
#include "../Device.h"

namespace phx {
class BufferManager {
    using FlushFunction = void (*)(ltl::movable_any &, vk::PipelineStageFlags, vk::AccessFlags);

  public:
    BufferManager(Device &device) noexcept : m_device{device} {}

    template <typename T, VkBufferUsageFlags flags>
    auto send(const std::vector<T> &values) {
        auto &bufferList = getBufferList<T, flags>();

        return bufferList.send(values);
    }

    void flush(vk::PipelineStageFlags dstStage, vk::AccessFlags dstAccess) noexcept {
        for (auto &&[any, flushFunction] : m_buffers | ltl::get(2_n, 3_n)) {
            flushFunction(any, dstStage, dstAccess);
        }
    }

  private:
    template <typename T, VkBufferUsageFlags _Flags>
    auto &getBufferList() noexcept {
        constexpr auto Flags = vk::BufferUsageFlags(_Flags);
        auto isCompatible = ltl::unzip([Flags](auto type_index, auto flags, auto &&, auto &&) { //
            return type_index == typeid(T) && flags == Flags;
        });

        if (auto *ptr = ltl::find_if_ptr(m_buffers, isCompatible)) {
            return (*ptr)[2_n].template get<BufferList<BufferInfo<T, _Flags>>>();
        }
        return createNewBuffer<T, _Flags>();
    }

    template <typename T, VkBufferUsageFlags _Flags>
    auto &createNewBuffer() {
        BufferList<BufferInfo<T, _Flags>> buffer{m_device};

        auto flush = [](ltl::movable_any &any, vk::PipelineStageFlags dstStage, vk::AccessFlags dstAccess) {
            any.get<BufferList<BufferInfo<T, _Flags>>>().flush(dstStage, dstAccess);
        };

        auto &tuple = m_buffers.emplace_back(typeid(T), vk::BufferUsageFlags(_Flags), std::move(buffer), flush);
        return tuple[2_n].template get<BufferList<BufferInfo<T, _Flags>>>();
    }

  private:
    Device &m_device;
    std::vector<ltl::tuple_t<std::type_index, vk::BufferUsageFlags, ltl::movable_any, FlushFunction>> m_buffers;
};
} // namespace phx
