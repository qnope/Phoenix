#include "GetDrawBatchesVisitor.h"

namespace phx {

namespace details {
DrawBatche GetDrawBatchesVisitorImpl::operator()(const GeometryNode &node) const
    noexcept {
  return {glm::mat4{1.0f}, node.drawInformations(), node.material()};
}
} // namespace details

GetDrawBatchesVisitor makeGetDrawBatchesVisitor() {
  return GetDrawBatchesVisitor{details::GetDrawBatchesVisitorImpl{}};
}

} // namespace phx
