#pragma once

#include "../DrawInformations/DrawInformations.h"
#include "../Materials/Material.h"
#include "../Nodes/GeometryNode.h"
#include "ListResultVisitor.h"
#include <vkw/vulkan.h>

namespace phx {

using DrawBatche = ltl::tuple_t<glm::mat4, DrawInformations, Material>;

struct GetDrawBatchesVisitor
    : ListResultVisitor<DrawBatche, GetDrawBatchesVisitor &, GeometryNode> {
  GetDrawBatchesVisitor() noexcept;
  DrawBatche operator()(const GeometryNode &node) const noexcept;

private:
  glm::mat4 m_matrix{1.0f};
};

} // namespace phx
