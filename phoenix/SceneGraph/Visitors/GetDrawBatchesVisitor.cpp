#include "GetDrawBatchesVisitor.h"

namespace phx {

GetDrawBatchesVisitor::GetDrawBatchesVisitor() noexcept
    : ListResultVisitor<MatrixAndDrawBatche, GetDrawBatchesVisitor &,
                        GeometryNode>{*this} {}

MatrixAndDrawBatche
GetDrawBatchesVisitor::operator()(const GeometryNode &node) const noexcept {
  return {m_matrix, {node.drawInformations(), node.material()}};
}

} // namespace phx
