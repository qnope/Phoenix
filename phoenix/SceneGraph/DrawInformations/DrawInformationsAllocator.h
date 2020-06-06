#pragma once

#include "DrawInformations.h"
#include <memory>
#include <vkw/Vertex.h>

namespace phx {

namespace details {
class DrawInformationsAllocatorImpl;
}

class Device;

class DrawInformationsAllocator {
public:
  DrawInformationsAllocator(Device &device);

  ~DrawInformationsAllocator();

  DrawInformations allocate(const std::vector<Complete3dVertex> &vertices,
                            const std::vector<uint32_t> &indices);

  void flush(vk::PipelineStageFlags nextPipeline,
             vk::AccessFlags nextAccess) noexcept;

private:
  std::unique_ptr<details::DrawInformationsAllocatorImpl>
      m_vertexAndIndexBufferListPimpl;
};

} // namespace phx
