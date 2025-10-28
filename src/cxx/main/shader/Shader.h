//
// Created by 30367 on 2025/10/28.
//

#ifndef INITIALGL_SHADER_H
#define INITIALGL_SHADER_H
#include "GL/glew.h"


class Shader {
public:
    virtual ~Shader() = default;
    virtual void compile() = 0;
    virtual GLuint getId() = 0;
    virtual bool isCompiled() = 0;
};


#endif //INITIALGL_SHADER_H