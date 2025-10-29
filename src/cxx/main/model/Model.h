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
class Model {
private:
    GLuint VAO, VBO;
    GLuint vertexCount;
    std::string path;
    bool loaded = false;
    glm::mat4 mModel;
public:
    explicit Model(std::string path);
    ~Model();
    void load();
    void draw();
    bool isLoaded();
    GLuint getVertexCount();
    void bindVAO();
    void unbindVAO();
    GLuint getVAO();
    const glm::mat4 &getModelMatrix();
    const glm::mat4 &setModelMatrix(const glm::mat4 &model);
};


#endif //INITIALGL_MODEL_H