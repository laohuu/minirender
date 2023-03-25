#ifndef RAYTRACING_MODEL_H
#define RAYTRACING_MODEL_H

#include "mesh.h"
#include "rasterizer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <enable_stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class model {
public:

    model(string const &path, bool gamma = false) {
        load_model(path);
    }

    void draw(rasterizer &raster);

private:
    vector<shared_ptr<texture>> textures_loaded;
    vector<mesh> meshes;
    string directory;

    void load_model(string path);

    void process_node(aiNode *node, const aiScene *scene);

    mesh process_mesh(aiMesh *mesh, const aiScene *scene);

    shared_ptr<texture> load_material_textures(aiMaterial *mat, aiTextureType type,
                                               string typeName);
};

void model::draw(rasterizer &raster) {
    for (auto &meshe: meshes)
        meshe.draw(raster);
}

void model::load_model(string path) {
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene);
}

void model::process_node(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

mesh model::process_mesh(aiMesh *ai_mesh, const aiScene *scene) {
    vector<vertex> vertices;
    vector<unsigned int> indices;
    vector<shared_ptr<texture>> textures;

    for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {
        vertex vertex;
        glm::vec3 vector;
        // positions
        vector.x = ai_mesh->mVertices[i].x;
        vector.y = ai_mesh->mVertices[i].y;
        vector.z = ai_mesh->mVertices[i].z;
        vertex.position = glm::vec4(vector, 1.0f);
        // normals
        if (ai_mesh->HasNormals()) {
            vector.x = ai_mesh->mNormals[i].x;
            vector.y = ai_mesh->mNormals[i].y;
            vector.z = ai_mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (ai_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = ai_mesh->mTextureCoords[0][i].x;
            vec.y = ai_mesh->mTextureCoords[0][i].y;
            vertex.texcoord = vec;

            // tangent
            if (ai_mesh->mTangents) {
                vector.x = ai_mesh->mTangents[i].x;
                vector.y = ai_mesh->mTangents[i].y;
                vector.z = ai_mesh->mTangents[i].z;
                vertex.tangent = vector;
            }

            // bitangent
            if (ai_mesh->mBitangents) {
                vector.x = ai_mesh->mBitangents[i].x;
                vector.y = ai_mesh->mBitangents[i].y;
                vector.z = ai_mesh->mBitangents[i].z;
                vertex.bitangent = vector;
            }
        } else {
            vertex.texcoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
        aiFace face = ai_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial *ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];
    // 1. diffuse maps
    shared_ptr<texture> diffuse_texture = load_material_textures(ai_material, aiTextureType_DIFFUSE,
                                                                 "texture_diffuse");
    // 2. specular maps
    shared_ptr<texture> specular_texture = load_material_textures(ai_material, aiTextureType_SPECULAR,
                                                                  "texture_specular");
    // 3. normal maps
    shared_ptr<texture> normal_texture = load_material_textures(ai_material, aiTextureType_HEIGHT,
                                                                "texture_normal");
//    // 4. height maps
//    std::vector<shared_ptr<texture>> height_texture = load_material_textures(ai_material, aiTextureType_AMBIENT,
//                                                                         "texture_height");

    shared_ptr<material> mat = make_shared<lambertian>(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    if (diffuse_texture != nullptr)
        mat = make_shared<lambertian>(diffuse_texture, specular_texture, normal_texture);
    // return a mesh object created from the extracted mesh data
    return mesh(vertices, indices, mat);
}

shared_ptr<texture> model::load_material_textures(aiMaterial *mat, aiTextureType type, string typeName) {
    if (mat->GetTextureCount(type) <= 0)
        return nullptr;
    shared_ptr<texture> texture_ptr;

    aiString str;
    mat->GetTexture(type, 0, &str);
    for (const auto &texture_loaded: textures_loaded) {
        if (std::strcmp(texture_loaded->path.data(), str.C_Str()) == 0) {
            return texture_loaded;
        }
    }
    string filename = string(str.C_Str());
    filename = this->directory + '/' + filename;
    auto tex = make_shared<image_texture>(filename.c_str());
    tex->type = typeName;
    tex->path = str.C_Str();
    textures_loaded.push_back(tex);
    return tex;
}


#endif //RAYTRACING_MODEL_H
