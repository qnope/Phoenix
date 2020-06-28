#pragma once

#include <cstdint>

#include <vkw/Buffer/BufferRef.h>

namespace phx {
struct DrawInformations {
    DrawInformations(IndexBufferRef<uint32_t> indexBuffer, VertexBufferRef<Complete3dVertex> vertexBuffer,
                     uint32_t indexCount, uint32_t firstIndex, uint32_t vertexOffset) :
        indexBuffer{indexBuffer},   //
        vertexBuffer{vertexBuffer}, //
        indexCount{indexCount},     //
        firstIndex{firstIndex},     //
        vertexOffset{vertexOffset} {}

    IndexBufferRef<uint32_t> indexBuffer;
    VertexBufferRef<Complete3dVertex> vertexBuffer;

    uint32_t indexCount;
    uint32_t firstIndex;
    uint32_t vertexOffset;

    friend bool operator<(const DrawInformations &drawInformations1,
                          const DrawInformations &drawInformations2) noexcept {
        auto ibo1 = drawInformations1.indexBuffer.getHandle();
        auto ibo2 = drawInformations2.indexBuffer.getHandle();
        auto vbo1 = drawInformations1.vertexBuffer.getHandle();
        auto vbo2 = drawInformations2.vertexBuffer.getHandle();

        if (ibo1 == ibo2)
            return vbo1 < vbo2;
        return ibo1 < ibo2;
    }
};
} // namespace phx
