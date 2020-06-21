#pragma once

#include "Nodes/GeometryNode.h"

namespace phx {

class SceneGraph;
class MaterialFactory;

std::vector<GeometryNode> loadModel(const std::string &path, SceneGraph &sceneGraph, MaterialFactory &materialFactory);

} // namespace phx
