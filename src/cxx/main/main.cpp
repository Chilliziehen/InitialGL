//
// Created by 30367 on 2025/10/28.
//
#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "config\config.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <cmath>

#include "FragmentShader.h"
#include "VertexShader.h"
#include "program/Program.h"

GLfloat vertices[] =
{
    -1.5f,-1.5f, -0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -1.5f, 1.5f, -0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
     1.5f, 1.5f, -0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
     1.5f,-1.5f, -0.0f,   0.5f, 0.5f, 0.0f,   1.0f, 0.0f

};
GLfloat cube[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
GLuint indices[] =
{
    0,1,2,
    2,0,3
};

int main() {
    try {
        glfwInit();
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* hWindow = glfwCreateWindow(1920, 1080, "Hello OpenGLbyChilliziehen", nullptr, nullptr);
        if (hWindow == nullptr)
        {
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(hWindow);
        glewExperimental = GL_TRUE;
        glewInit();
        if (glewInit() != GLEW_OK)
        {
            throw std::runtime_error("Failed to initialize GLEW");
        }
        int ViewHeight;
        int ViewWidth;
        glfwGetFramebufferSize(hWindow, &ViewWidth, &ViewHeight);
        glViewport(0, 0, ViewWidth, ViewHeight);

        GLuint VAO, VBO, EBO;
        //建立VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        //建立VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
        //建立属性索引
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        //解绑该VAO
        glBindBuffer(GL_ARRAY_BUFFER, NULL);

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        {
            int texWidth, texHeight,nrChannels;
            std::string resourcePath = std::string(RESOURCE_PATH)+std::string("/container.jpg");
            std::cout<<resourcePath<<std::endl;
            unsigned char* image = stbi_load(resourcePath.c_str(), &texWidth, &texHeight, &nrChannels, 0);
            if (image!= nullptr)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                throw std::runtime_error("Failed to load texture");
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            stbi_image_free(image);
        }
        glBindTexture(GL_TEXTURE_2D,NULL);
        //建立绘制索引
        //glGenBuffers(1, &EBO);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

        auto uLocMat4View = glGetUniformLocation(0, "umat4View");;
        auto uLocMat4Proj = glGetUniformLocation(0, "umat4Proj");
        auto uLocMat4Model = glGetUniformLocation(0, "umat4Model");
        auto uLocMat4Local = glGetUniformLocation(0, "umat4Local");

        glm::mat4 mat4View = glm::mat4(1.0f);
        glm::mat4 mat4Proj = glm::perspective(glm::radians(45.0f), (float)ViewWidth / (float)ViewHeight, 0.1f, 100.0f);
        glm::mat4 mat4Model = glm::mat4(1.0f);
        glm::mat4 mat4Local = glm::mat4(1.0f);

        glUniform4fv(uLocMat4View, 1, glm::value_ptr(mat4View));
        glUniform4fv(uLocMat4Proj, 1, glm::value_ptr(mat4Proj));
        glUniform4fv(uLocMat4Model, 1, glm::value_ptr(mat4Model));
        glUniform4fv(uLocMat4Local, 1, glm::value_ptr(mat4Local));

        std::string vshPath = std::string(GLSL_PATH) + std::string("/vertex.vsh");
        std::string fshPath = std::string(GLSL_PATH) + std::string("/fragment.fsh");
        VertexShader vsh(vshPath.c_str());
        vsh.compile();

        FragmentShader fsh(fshPath.c_str());
        fsh.compile();

        Program p;
        p.init();
        p.addShader(&vsh,GL_VERTEX_SHADER);
        p.addShader(&fsh,GL_FRAGMENT_SHADER);
        p.compileAll();
        p.linkAll();
        if (!p.checkReady())
            throw std::runtime_error("Program not ready");
        p.use();
        while (!glfwWindowShouldClose(hWindow))
        {
            glfwPollEvents();
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            //绑定纹理
            glBindTexture(GL_TEXTURE_2D, texture);
            //绘制
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(NULL);

            glfwSwapBuffers(hWindow);
        }
    }
    catch (const std::exception& e) {
        std::cerr<<"[Main] Exception: " << e.what() << std::endl;
        return -1;
    }
}