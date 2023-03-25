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

    vector<shared_ptr<texture>> load_material_textures(aiMaterial *mat, aiTextureType type,
                                                       string typeName);
};

void model::draw(rasterizer &raster) {
    for (auto & meshe : meshes)
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

    // walk through each of the mesh's vertices
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
    aiMaterial *material = scene->mMaterials[ai_mesh->mMaterialIndex];

    // 1. diffuse maps
    vector<shared_ptr<texture>> diffuseMaps = load_material_textures(material, aiTextureType_DIFFUSE,
                                                                     "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<shared_ptr<texture>> specularMaps = load_material_textures(material, aiTextureType_SPECULAR,
                                                                      "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<shared_ptr<texture>> normalMaps = load_material_textures(material, aiTextureType_HEIGHT,
                                                                         "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<shared_ptr<texture>> heightMaps = load_material_textures(material, aiTextureType_AMBIENT,
                                                                         "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return mesh(vertices, indices, textures);
}

void texture_from_file(const char *path, const string &directory, shared_ptr<texture> &tex) {
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned char *data = stbi_load(filename.c_str(), &tex->width, &tex->height, &tex->channel, 0);
    if (data) {
        tex->set_color(data);
        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
}

vector<shared_ptr<texture>> model::load_material_textures(aiMaterial *mat, aiTextureType type, string typeName) {
    vector<shared_ptr<texture>> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j]->path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            shared_ptr<texture> tex = make_shared<texture>();
            texture_from_file(str.C_Str(), this->directory, tex);
            std::cout << str.C_Str() << std::endl;
            tex->type = typeName;
            tex->path = str.C_Str();
            textures.push_back(tex);
            textures_loaded.push_back(tex);
        }
    }
    return textures;
}


#endif //RAYTRACING_MODEL_H
