#pragma once

#include "../DrawInformations/DrawInformations.h"
#include "../Materials/Material.h"
#include "../Nodes/GeometryNode.h"
#include "ListResultVisitor.h"
#include <vkw/vulkan.h>

namespace phx {

using DrawBatche = ltl::tuple_t<DrawInformations, Material>;
using MatrixAndDrawBatche = ltl::tuple_t<glm::mat4, DrawBatche>;

struct GetDrawBatchesVisitor
    : ListResultVisitor<MatrixAndDrawBatche, GetDrawBatchesVisitor &,
                        GeometryNode> {
  GetDrawBatchesVisitor() noexcept;
  MatrixAndDrawBatche operator()(const GeometryNode &node) const noexcept;

private:
  glm::mat4 m_matrix{1.0f};
};

} // namespace phx
