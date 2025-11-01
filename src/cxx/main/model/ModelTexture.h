//
// Created by 30367 on 2025/11/1.
//

#ifndef INITIALGL_MODELTEXTURE_H
#define INITIALGL_MODELTEXTURE_H

#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

enum class TextureType {
    DIFFUSE,
    SPECULAR
};

struct TextureCreateInfo {
    GLint wrapS;
    GLint wrapT;
    GLint minFilter;
    GLint magFilter;
};

class ModelTexture {
private:
    struct TextureParam {
        GLint width;
        GLint height;
        GLint nrChannels;
    };

    GLuint textureID;
    TextureType textureType;
    std::string texturePath;
    TextureParam textureParam;
    unsigned char* textureData;
    GLint textureIndex;

public:
    explicit ModelTexture(TextureType textureType);
    ~ModelTexture();
    void preloadTexture(const std::string& path);
    void releaseMemory();

    GLint setTextureIndex(const GLint& index);

    /*
     * 加载纹理到 GPU 并生成 OpenGL 纹理对象
     */
    void loadTexture(const TextureCreateInfo& textureInfo);

    void bindTexture();
    void unbindTexture();
    void unloadTexture();
};


#endif //INITIALGL_MODELTEXTURE_H