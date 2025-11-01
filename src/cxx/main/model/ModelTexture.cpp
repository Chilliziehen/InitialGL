//
// Created by 30367 on 2025/11/1.
//

#include "ModelTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stdexcept>

#include "stb_image.h"

#include "src/cxx/main/predefinition/Predefinitions.h"

ModelTexture::ModelTexture(TextureType type) : textureType(type),textureIndex(-1),textureData(nullptr),textureID(NULL) {
    this->textureParam.width = NULL;
    this->textureParam.height = NULL;
    this->textureParam.nrChannels = NULL;
}

ModelTexture::~ModelTexture() {
    if (this->textureData != nullptr) {
        stbi_image_free(this->textureData);
        this->textureData = nullptr;
    }
    if (this->textureID != NULL) {
        glDeleteTextures(1, &this->textureID);
    }
}

GLint ModelTexture::setTextureIndex(const GLint &index) {
    return this->textureIndex = index;
}

void ModelTexture::preloadTexture(const std::string &path) {
    this->texturePath = path;
    stbi_set_flip_vertically_on_load(true);
    this->textureData = stbi_load(path.c_str(), &this->textureParam.width, &this->textureParam.height, &this->textureParam.nrChannels, 0);
    if (this->textureData == nullptr) {
        throw std::runtime_error("[ModelTexture] Failed to load texture at path: " + path);
    }
}

void ModelTexture::loadTexture(const TextureCreateInfo& info) {
    this->textureID = NULL;
    glGenTextures(1, &this->textureID);
    if (this->textureIndex == -1) {
        throw std::runtime_error("[ModelTexture] Texture index not set before loading texture.");
    }
    glActiveTexture(this->textureIndex);
    glBindTexture(GL_TEXTURE_2D, this->textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, info.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, info.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, info.magFilter);
    GLint format = (this->textureParam.nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, this->textureParam.width, this->textureParam.height, 0, format, GL_UNSIGNED_BYTE, this->textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
}

void ModelTexture::bindTexture() {
    if (this->textureIndex == -1) {
        throw std::runtime_error("[ModelTexture] Texture index not set before binding texture.");
    }
    glActiveTexture(this->textureIndex);
    glBindTexture(GL_TEXTURE_2D, this->textureID);
}

void ModelTexture::unbindTexture() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ModelTexture::unloadTexture() {
    glDeleteTextures(1, &this->textureID);
    this->textureIndex = -1;
    this->textureID = NULL;
}

void ModelTexture::releaseMemory() {
    if (this->textureData != nullptr) {
        stbi_image_free(this->textureData);
        this->textureData = nullptr;
    }
}