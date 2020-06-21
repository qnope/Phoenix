#include "ModelImporter.h"
#include <vkw/utility.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "SceneGraph.h"
#include "Materials/MaterialFactory.h"

#include <vkw/Vertex.h>

#include <ltl/operator.h>
#include <ltl/functional.h>
#include <ltl/range/zip.h>

namespace phx {

using SubDrawInfos = ltl::tuple_t<std::vector<Complete3dVertex>, std::vector<uint32_t>>;

static std::vector<aiMesh *> getMeshes(const aiScene *scene) noexcept {
    return {scene->mMeshes, scene->mMeshes + scene->mNumMeshes};
}

static std::vector<aiMaterial *> getMaterials(const aiScene *scene) noexcept {
    return {scene->mMaterials, scene->mMaterials + scene->mNumMaterials};
}

static std::vector<Complete3dVertex> meshToVertices(aiMesh *mesh) noexcept {
    std::vector<Complete3dVertex> vertices;
    auto toGlm = [](aiVector3D v) { return glm::vec3{v.x, v.y, v.z}; };

    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        Complete3dVertex vertex;
        vertex.position = toGlm(mesh->mVertices[i]);
        vertex.normal = toGlm(mesh->mNormals[i]);
        vertex.texCoord = toGlm(mesh->mTextureCoords[0][i]).xy();
        vertex.tangeant = toGlm(mesh->mTangents[i]);
        vertex.biTangeant = toGlm(mesh->mBitangents[i]);
        vertices.emplace_back(vertex);
    }

    return vertices;
}

static std::vector<uint32_t> meshToIndices(aiMesh *mesh) noexcept {
    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];

        indices.emplace_back(face.mIndices[0]);
        indices.emplace_back(face.mIndices[1]);
        indices.emplace_back(face.mIndices[2]);
    }

    return indices;
}

static std::vector<ltl::tuple_t<SubDrawInfos, aiMaterial *>> zipMeshMaterial(const aiScene *scene) {
    auto meshes = getMeshes(scene);
    auto materials = getMaterials(scene);

    auto meshToMaterial = [&materials](aiMesh *mesh) { return materials[mesh->mMaterialIndex]; };

    auto indices = meshes | ltl::map(meshToIndices);
    auto vertices = meshes | ltl::map(meshToVertices);
    auto drawInfos = ltl::zip(vertices, indices) | ltl::map(ltl::construct_with_tuple<SubDrawInfos>());

    return ltl::zip(drawInfos, meshes | ltl::map(meshToMaterial));
}

static std::string normalizePathTexture(std::string _path) {
    std::string path = std::move(_path);
    if (path[0] == '.')
        path.erase(0, 1);

    if (path[0] == '/' || path[0] == '\\')
        path.erase(0, 1);

    for (auto &v : path)
        if (v == '\\')
            v = '/';
    return path;
}

Material createMaterial(MaterialFactory &materialFactory, const std::string &globalPath, const aiMaterial *material) {
    aiString texPath;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
        return materialFactory.createTexturedLambertianMaterial(globalPath + normalizePathTexture(texPath.C_Str()));
    }

    aiColor3D color;

    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        return materialFactory.createColoredLambertianMaterial(glm::vec4(color.r, color.g, color.b, 1.0));
    }

    return materialFactory.createColoredLambertianMaterial(glm::vec4(0.0));
}

static GeometryNode toGeometryNode(SceneGraph &sceneGraph, MaterialFactory &materialFactory,
                                   const std::string &globalPath,
                                   const ltl::tuple_t<SubDrawInfos, aiMaterial *> &values) {
    DrawInformations informations = sceneGraph.allocateDrawInformations(values[0_n][0_n], values[0_n][1_n]);
    auto material = createMaterial(materialFactory, globalPath, values[1_n]);
    return {informations, material};
}

static std::string pathToGlobalPath(std::string path) {
    std::size_t last = path.find_last_of('/');

    if (last == std::string::npos) {
        return "./";
    }

    path.erase(last);
    path += "/";

    return path;
}

std::vector<phx::GeometryNode> loadModel(const std::string &path, SceneGraph &sceneGraph,
                                         MaterialFactory &materialFactory) {
    Assimp::Importer importer;

    auto *scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

    if (scene == nullptr)
        throw FileNotFoundException{path};

    auto globalPath = pathToGlobalPath(path);
    auto drawInfosAndMaterial = zipMeshMaterial(scene);

    return drawInfosAndMaterial |
           ltl::map(ltl::curry(&toGeometryNode, std::ref(sceneGraph), std::ref(materialFactory), globalPath));
}

} // namespace phx
