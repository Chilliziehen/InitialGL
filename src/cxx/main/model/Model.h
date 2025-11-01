//
// Created by 30367 on 2025/10/28.
//

#ifndef INITIALGL_MODEL_H
#define INITIALGL_MODEL_H

#include "glm.hpp"
//#include "GL/eglew.h"
#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "Shader.h"

#include "src/cxx/main/model/ModelTexture.h"

class Model {
private:
    GLuint VAO, VBO;
    GLuint vertexCount;
    std::string path;
    bool loaded = false;
    glm::mat4 mModel;
    ModelTexture diffuseTextureObj{TextureType::DIFFUSE};
    ModelTexture specularTextureObj{TextureType::SPECULAR};
public:
    explicit Model(std::string path);
    ~Model();
    void load();
    void draw();
    bool isLoaded();
    GLuint getVertexCount();
    void bindVAO();
    void bindTexture();
    void unbindTexture();
    void drawModel();
    void unbindVAO();
    GLuint getVAO();
    const glm::mat4 &getModelMatrix();
    const glm::mat4 &setModelMatrix(const glm::mat4 &model);
    ModelTexture& getDiffuseTextureObj() { return diffuseTextureObj; };
    ModelTexture& getSpecularTextureObj() { return specularTextureObj; };
};


#endif //INITIALGL_MODEL_H