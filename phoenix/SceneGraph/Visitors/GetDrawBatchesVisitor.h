#pragma once

#include "../DrawInformations/DrawInformations.h"
#include "../Materials/Material.h"
#include "../Nodes/GeometryNode.h"
#include "ListResultVisitor.h"
#include <vkw/vulkan.h>

namespace phx {

using DrawBatche = ltl::tuple_t<glm::mat4, DrawInformations, Material>;

namespace details {
struct GetDrawBatchesVisitorImpl {
  DrawBatche operator()(const GeometryNode &node) const noexcept;
};
} // namespace details

using GetDrawBatchesVisitor =
    ListResultVisitor<DrawBatche, details::GetDrawBatchesVisitorImpl,
                      GeometryNode>;

GetDrawBatchesVisitor makeGetDrawBatchesVisitor();

} // namespace phx
