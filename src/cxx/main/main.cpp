//
// Created by 30367 on 2025/10/28.
//
#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "model\Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "config\config.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <cmath>

#include "FragmentShader.h"
#include "VertexShader.h"
#include "camera/Camera.h"
#include "program/Program.h"
#include "call_back/CallBack.h"
#include "iomanip"

#define WIDTH 1920
#define HEIGHT 1080

bool keys[1024] = {false};
float scale = 1.0f;
bool cursorEnabled = false;
Camera* cam;



int main() {
    try {
        //glDisable(GL_CULL_FACE);
        glfwInit();
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //创建相机
        cam = new Camera();

        GLFWwindow* hWindow = glfwCreateWindow(WIDTH, HEIGHT, "Hello OpenGLbyChilliziehen", nullptr, nullptr);
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
        int ViewHeight = HEIGHT;
        int ViewWidth = WIDTH;
        glfwGetFramebufferSize(hWindow, &ViewWidth, &ViewHeight);
        glViewport(0, 0, ViewWidth, ViewHeight);
        glEnable(GL_DEPTH_TEST);

        GLuint texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
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
            // 重复上传不必要，删除第二次 glTexImage2D
            stbi_image_free(image);
        }

        GLuint texture_specular;
        glGenTextures(1, &texture_specular);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_specular);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        {
            int texWidth, texHeight,nrChannels;
            std::string resourcePath = std::string(RESOURCE_PATH)+std::string("/container.jpg");
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
            // 重复上传不必要，删除第二次 glTexImage2D
            stbi_image_free(image);
        }

        //glBindTexture(GL_TEXTURE_2D, 0);
        //建立绘制索引
        //glGenBuffers(1, &EBO);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

        // 先创建、编译和链接程序
        std::string gVshPath = std::string(GLSL_PATH)+std::string("/gPass/gVertex.vert");
        std::string gFshPath = std::string(GLSL_PATH)+std::string("/gPass/gFragment.frag");
        VertexShader gVsh(gVshPath.c_str());
        FragmentShader gFsh(gFshPath.c_str());
        gVsh.compile();
        gFsh.compile();
        Program gPass;
        gPass.init();
        gPass.addShader(&gVsh, GL_VERTEX_SHADER);
        gPass.addShader(&gFsh, GL_FRAGMENT_SHADER);
        gPass.compileAll();
        gPass.linkAll();

        std::string lVshPath = std::string(GLSL_PATH)+std::string("/Pass/lVertex.vert");
        std::string lFshPath = std::string(GLSL_PATH)+std::string("/Pass/lFragment.frag");
        VertexShader lVsh(lVshPath.c_str());
        FragmentShader lFsh(lFshPath.c_str());
        lVsh.compile();
        lFsh.compile();
        Program lPass;
        lPass.init();
        lPass.addShader(&lVsh, GL_VERTEX_SHADER);
        lPass.addShader(&lFsh, GL_FRAGMENT_SHADER);
        lPass.compileAll();
        lPass.linkAll();

        // 在 use 之后设置 Uniform（使用正确的 programId）
        GLuint gPassProgId = gPass.getId();
        GLuint lPassProgId = lPass.getId();

        gPass.use();
        GLint uLocMat4View = glGetUniformLocation(gPassProgId, "umat4View");
        GLint uLocMat4Proj = glGetUniformLocation(gPassProgId, "umat4Proj");
        GLint uLocMat4Model = glGetUniformLocation(gPassProgId, "umat4Model");
        GLint uLocTDiffuse1 = glGetUniformLocation(gPassProgId, "texture_diffuse1");
        GLint uLocTSpecular1 = glGetUniformLocation(gPassProgId, "texture_specular1");

        glm::mat4 mat4View = glm::lookAt(glm::vec3(1.0f,1.0f,3.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f)); //cam->getViewMatrix();
        glm::mat4 mat4Proj = cam->getProjectionMatrix();//glm::perspective(45.0f, 1920.f / 1080.0f, 0.1f, 10000.0f);
        glm::mat4 mat4Model = glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f));

        if (uLocMat4View != -1)
            glUniformMatrix4fv(uLocMat4View, 1, GL_FALSE, glm::value_ptr(mat4View));
        if (uLocMat4Proj != -1)
            glUniformMatrix4fv(uLocMat4Proj, 1, GL_FALSE, glm::value_ptr(mat4Proj));
        if (uLocMat4Model != -1)
            glUniformMatrix4fv(uLocMat4Model, 1, GL_FALSE, glm::value_ptr(mat4Model));
        if (uLocTDiffuse1 != -1)
            glUniform1i(uLocTDiffuse1, 0);
        if (uLocTSpecular1 != -1)
            glUniform1i(uLocTSpecular1, 1);


        glfwSetKeyCallback(hWindow, CallBack::keyboard_callback);
        glfwSetCursorPosCallback(hWindow, CallBack::mouse_callback);
        glfwSetScrollCallback(hWindow,CallBack::mouse_scroll_callback);

        Model teapot(std::string(RESOURCE_PATH)+std::string("/teamugobj.obj"));
        teapot.load();

        GLuint gBuffer;
        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        GLuint gPosition, gNormal, gAlbedoSpecular;

        // Position
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        // Normal
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        // Albedo+Spec
        glGenTextures(1, &gAlbedoSpecular);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpecular);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpecular, 0);

        // 指定颜色附件集合
        const GLenum attachments[3] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
        };
        glDrawBuffers(3, attachments);

        // 深度附件（Renderbuffer 或 深度纹理二选一）
        GLuint rboDepth;
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        // 完整性检查
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("GBuffer is not complete!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        while (!glfwWindowShouldClose(hWindow))
        {
            glfwPollEvents();
            glfwSetInputMode(hWindow, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            cam->move(keys);
            cam->mousemove();
            mat4View = cam->getViewMatrix();
                //绑定纹理到单元0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            if (uLocMat4View != -1)
                glUniformMatrix4fv(uLocMat4View, 1, GL_FALSE, glm::value_ptr(mat4View));
            //绘制
            teapot.bindVAO();
            if (uLocMat4Proj != -1) {
                glUniformMatrix4fv(uLocMat4Model,
                    1,
                    GL_FALSE,
                    glm::value_ptr(teapot.getModelMatrix()));
            }
            glDrawArrays(GL_TRIANGLES, 0, teapot.getVertexCount());
            teapot.unbindVAO();
            glBindVertexArray(NULL);
            glfwSwapBuffers(hWindow);
        }
    }
    catch (const std::exception& e) {
        std::cerr<<"[Main] Exception: " << e.what() << std::endl;
        return -1;
    }
}