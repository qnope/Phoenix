#include "DrawInformationsAllocator.h"

#include <vkw/Buffer/BufferList.h>
#include <vkw/Device.h>

namespace phx {

namespace details {

class DrawInformationsAllocatorImpl {
public:
  DrawInformationsAllocatorImpl(Device &device)
      : m_vertexBufferList{device}, //
        m_indexBufferList{device} {}

  DrawInformations allocate(const std::vector<Complete3dVertex> &vertices,
                            const std::vector<uint32_t> &indices) {
    auto vertexBufferInfo = m_vertexBufferList.send(vertices);
    auto indexBufferInfo = m_indexBufferList.send(indices);

    return DrawInformations(indexBufferInfo.buffer, vertexBufferInfo.buffer,
                            indexBufferInfo.size, indexBufferInfo.offset,
                            vertexBufferInfo.offset);
  }

private:
  BufferList<Complete3dVertexBufferInfo> m_vertexBufferList;
  BufferList<IndexBufferInfo> m_indexBufferList;
};

} // namespace details

DrawInformationsAllocator::DrawInformationsAllocator(Device &device)
    : m_vertexAndIndexBufferListPimpl{
          std::make_unique<details::DrawInformationsAllocatorImpl>(device)} {}

DrawInformationsAllocator::~DrawInformationsAllocator() = default;

DrawInformations DrawInformationsAllocator::allocate(
    const std::vector<Complete3dVertex> &vertices,
    const std::vector<uint32_t> &indices) {
  return m_vertexAndIndexBufferListPimpl->allocate(vertices, indices);
}

} // namespace phx
