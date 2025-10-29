//
// Created by 30367 on 2025/10/28.
//
#define TINYOBJLOADER_IMPLEMENTATION
#include "OTHER/MODEL/tiny_obj_loader.h"
#include "Model.h"
#include <stdexcept>
Model::Model(std::string path) :path(std::move(path)), loaded(false), VAO(0), VBO(0), vertexCount(0) {
    this->loaded = false;
    this->mModel = glm::mat4(1.0f);
}

Model::~Model() {
    if (loaded) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void Model::load() {
    try {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(),nullptr,true)) {
            throw std::runtime_error(std::string("[Model Loader][Load]")+err);
        }

        std::vector<float> vertexData;
        this->vertexCount = shapes[0].mesh.indices.size();

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                // 顶点坐标
                vertexData.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                vertexData.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                vertexData.push_back(attrib.vertices[3 * index.vertex_index + 2]);
                // 纹理坐标（可选）
                if (!attrib.texcoords.empty()&& index.texcoord_index >=0) {
                    vertexData.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                    vertexData.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
                } else {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                }
                // 法线（可选）
                if (!attrib.normals.empty()&& index.normal_index >=0) {
                    vertexData.push_back(attrib.normals[3 * index.normal_index + 0]);
                    vertexData.push_back(attrib.normals[3 * index.normal_index + 1]);
                    vertexData.push_back(attrib.normals[3 * index.normal_index + 2]);
                } else {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                    vertexData.push_back(1.0f);
                }
            }
        }

        //GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
        GLsizei stride = (3 + 2 + 3) * sizeof(float);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)((3 + 2) * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
        this->loaded = true;
    }
    catch (std::runtime_error& e) {
        throw std::runtime_error("[Model][Load] " + std::string(e.what()));
    }
}

void Model::draw() {
    if (!loaded) {
        throw std::runtime_error("[Model][Draw] Model not loaded.");
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

bool Model::isLoaded() {
    return loaded;
}

GLuint Model::getVertexCount() {
    return vertexCount;
}

void Model::bindVAO() {
    glBindVertexArray(VAO);
}

void Model::unbindVAO() {
    glBindVertexArray(0);
}

GLuint Model::getVAO() {
    return VAO;
}

const glm::mat4 &Model::getModelMatrix() {
    return mModel;
}

const glm::mat4 &Model::setModelMatrix(const glm::mat4 &model) {
    return this->mModel = model;
}