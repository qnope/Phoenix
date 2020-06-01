#pragma once

#include <cstdint>

#include <vkw/Buffer/BufferRef.h>

namespace phx {
struct DrawInformations {
  DrawInformations(IndexBufferRef<uint32_t> indexBuffer,
                   VertexBufferRef<Complete3dVertex> vertexBuffer,
                   uint32_t indexCount, uint32_t firstIndex,
                   uint32_t vertexOffset)
      : indexBuffer{indexBuffer},   //
        vertexBuffer{vertexBuffer}, //
        indexCount{indexCount},     //
        firstIndex{firstIndex},     //
        vertexOffset{vertexOffset} {}

  IndexBufferRef<uint32_t> indexBuffer;
  VertexBufferRef<Complete3dVertex> vertexBuffer;

  uint32_t indexCount;
  uint32_t firstIndex;
  uint32_t vertexOffset;
};
} // namespace phx
