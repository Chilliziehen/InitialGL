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
        // 初始化 GLEW（仅一次）
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
            stbi_image_free(image);
        }

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

        // 修正 lPass 路径（原来是 /Pass/）
        std::string lVshPath = std::string(GLSL_PATH)+std::string("/lPass/lVertex.vert");
        std::string lFshPath = std::string(GLSL_PATH)+std::string("/lPass/lFragment.frag");
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

        std::string sVshPath = std::string(GLSL_PATH)+std::string("/sPass/sVertex.vert");
        std::string sFshPath = std::string(GLSL_PATH)+std::string("/sPass/sFragment.frag");
        VertexShader sVsh(sVshPath.c_str());
        FragmentShader sFsh(sFshPath.c_str());
        sVsh.compile();
        sFsh.compile();
        Program shadowPass;
        shadowPass.init();
        shadowPass.addShader(&sVsh, GL_VERTEX_SHADER);
        shadowPass.addShader(&sFsh, GL_FRAGMENT_SHADER);
        shadowPass.compileAll();
        shadowPass.linkAll();

        // 在 use 之后设置 Uniform（使用正确的 programId）
        GLuint gPassProgId = gPass.getId();
        GLuint lPassProgId = lPass.getId();
        GLuint sPassProgId = shadowPass.getId();

        // gPass 采样器绑定到纹理单元 0/1
        gPass.use();
        GLint uLocMat4View = glGetUniformLocation(gPassProgId, "umat4View");
        GLint uLocMat4Proj = glGetUniformLocation(gPassProgId, "umat4Proj");
        GLint uLocMat4Model = glGetUniformLocation(gPassProgId, "umat4Model");
        GLint uLocTDiffuse1 = glGetUniformLocation(gPassProgId, "texture_diffuse1");
        GLint uLocTSpecular1 = glGetUniformLocation(gPassProgId, "texture_specular1");
        if (uLocTDiffuse1 != -1) glUniform1i(uLocTDiffuse1, 0);
        if (uLocTSpecular1 != -1) glUniform1i(uLocTSpecular1, 1);

        // lPass 采样器固定绑定到纹理单元 0/1/2
        lPass.use();
        GLint uLocGPos = glGetUniformLocation(lPassProgId, "gPosition");
        GLint uLocGNormal = glGetUniformLocation(lPassProgId, "gNormal");
        GLint uLocGAlbedoSpec = glGetUniformLocation(lPassProgId, "gAlbedoSpec");
        GLint uLocSTexture = glGetUniformLocation(lPassProgId, "shadowTexture");
        GLint uLocGainTexture = glGetUniformLocation(lPassProgId, "gainTexture");
        GLint uLocMat4LightProj_lPass = glGetUniformLocation(lPassProgId, "umat4LightProj");
        GLint uLocMat4LightView_lPass = glGetUniformLocation(lPassProgId, "umat4LightView");
        if (uLocGPos != -1) glUniform1i(uLocGPos, 0);
        if (uLocGNormal != -1) glUniform1i(uLocGNormal, 1);
        if (uLocGAlbedoSpec != -1) glUniform1i(uLocGAlbedoSpec, 2);
        if (uLocSTexture != -1) glUniform1i(uLocSTexture, 3);
        if (uLocGainTexture != -1) glUniform1i(uLocGainTexture, 4);

        shadowPass.use();
        GLint uLocMat4LightView = glGetUniformLocation(sPassProgId, "umat4LightView");
        GLint uLocMat4LightProj = glGetUniformLocation(sPassProgId, "umat4LightProj");
        GLint uLocMat4ModelShadow = glGetUniformLocation(sPassProgId, "umat4Model");

        // 相机矩阵初值
        glm::mat4 mat4View = glm::lookAt(glm::vec3(1.0f,1.0f,3.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 mat4Proj = cam->getProjectionMatrix();
        glm::mat4 mat4Model = glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f));
        glm::mat4 mat4LightView = glm::lookAt(glm::vec3(2.0f,2.0f,2.0f),
            glm::vec3(0.0f,0.0f,0.0f),glm::normalize(glm::cross(glm::vec3(-1.0f,0.0f,1.0f),glm::vec3(-1.0f,-1.0f,-1.0f))));

        glm::mat4 mat4LightProj = glm::ortho(-15.0f,
        15.0f,
            -8.0f,
            8.0f,
            0.1f,
            100.0f);

        //glm::mat4 mat4LightProj = glm::perspective(glm::radians(60.0f),1.0f,0.1f,100.0f);
        gPass.use();
        if (uLocMat4View != -1)
            glUniformMatrix4fv(uLocMat4View, 1, GL_FALSE, glm::value_ptr(mat4View));
        if (uLocMat4Proj != -1)
            glUniformMatrix4fv(uLocMat4Proj, 1, GL_FALSE, glm::value_ptr(mat4Proj));
        if (uLocMat4Model != -1)
            glUniformMatrix4fv(uLocMat4Model, 1, GL_FALSE, glm::value_ptr(mat4Model));

        glfwSetKeyCallback(hWindow, CallBack::keyboard_callback);
        glfwSetCursorPosCallback(hWindow, CallBack::mouse_callback);
        glfwSetScrollCallback(hWindow,CallBack::mouse_scroll_callback);

        Model teapot(std::string(RESOURCE_PATH)+std::string("/teamugobj.obj"));
        teapot.setModelMatrix(glm::translate(glm::mat4(1.0f),glm::vec3(0.3f,-0.2f,0.3f))*glm::scale(glm::mat4(1.0f),glm::vec3(0.5f,0.5f,0.5f)));
        teapot.load();

        Model plane(std::string(RESOURCE_PATH)+std::string("/plane.obj"));
        plane.load();
        plane.setModelMatrix(glm::translate(glm::mat4(1.0f),glm::vec3(-500.0f,-0.2f,-500.0f))*glm::scale(glm::mat4(1.0f),glm::vec3(1000.0f,1000.0f,1000.0f)));

        // 创建 G-Buffer
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
        //指定该纹理为颜色附件0
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
        //在片元着色器的layout中指定输出位置时要对应这里的顺序
        const GLenum attachments[3] = {
            GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
        };
        glDrawBuffers(3, attachments);

        // 深度附件（Renderbuffer）
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

        // 创建阴影贴图
        GLuint sBuffer;
        glGenFramebuffers(1, &sBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, sBuffer);
        // 创建深度纹理
        GLuint depthTexture;
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        //创建光源平行光增益贴图
        GLuint lGainTexture;
        glGenTextures(1, &lGainTexture);
        glBindTexture(GL_TEXTURE_2D, lGainTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        //绑定到FBO的深度输出
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        //绑定到FBO的颜色输出
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lGainTexture, 0);
        const GLenum sAttachments[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1,sAttachments);

        // 构建全屏 Quad（NDC xy, UV）
        GLuint quadVAO = 0, quadVBO = 0;
        {
            float quadVertices[] = {
                // x, y, u, v
                -1.0f, -1.0f, 0.0f, 0.0f,
                 1.0f, -1.0f, 1.0f, 0.0f,
                 1.0f,  1.0f, 1.0f, 1.0f,

                -1.0f, -1.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 1.0f, 1.0f,
                -1.0f,  1.0f, 0.0f, 1.0f
            };
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glBindVertexArray(0);
        }

        while (!glfwWindowShouldClose(hWindow))
        {
            glfwPollEvents();
            glfwSetInputMode(hWindow, GLFW_CURSOR, cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

            cam->move(keys);
            cam->mousemove();
            mat4View = cam->getViewMatrix();
            mat4Proj = cam->getProjectionMatrix();

            //shadow pass, write into S-Buffer.
            glBindFramebuffer(GL_FRAMEBUFFER, sBuffer);
            glViewport(0, 0, ViewWidth, ViewHeight);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            shadowPass.use();
            //传入参数
            if (uLocMat4LightView != -1)
                glUniformMatrix4fv(uLocMat4LightView, 1, GL_FALSE, glm::value_ptr(mat4LightView));
            if (uLocMat4LightProj != -1)
                glUniformMatrix4fv(uLocMat4LightProj, 1, GL_FALSE, glm::value_ptr(mat4LightProj));
            //Under construction.
            teapot.bindVAO();
            glUniformMatrix4fv(uLocMat4ModelShadow, 1, GL_FALSE, glm::value_ptr(teapot.getModelMatrix()));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(teapot.getVertexCount()));
            teapot.unbindVAO();
            plane.bindVAO();
            glUniformMatrix4fv(uLocMat4ModelShadow, 1, GL_FALSE, glm::value_ptr(plane.getModelMatrix()));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(plane.getVertexCount()));
            plane.unbindVAO();

            // 几何通道：写入 G-Buffer
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glViewport(0, 0, ViewWidth, ViewHeight);
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            gPass.use();
            if (uLocMat4View != -1)
                glUniformMatrix4fv(uLocMat4View, 1, GL_FALSE, glm::value_ptr(mat4View));
            if (uLocMat4Proj != -1)
                glUniformMatrix4fv(uLocMat4Proj, 1, GL_FALSE, glm::value_ptr(mat4Proj));

            // 绑定漫反射与高光贴图
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_specular);

            plane.bindVAO();
            glUniformMatrix4fv(uLocMat4Model, 1, GL_FALSE, glm::value_ptr(plane.getModelMatrix()));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(plane.getVertexCount()));
            plane.unbindVAO();

            teapot.bindVAO();
            glUniformMatrix4fv(uLocMat4Model, 1, GL_FALSE, glm::value_ptr(teapot.getModelMatrix()));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(teapot.getVertexCount()));
            teapot.unbindVAO();

            // 光照通道：读取 G-Buffer 合成
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, ViewWidth, ViewHeight);
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            lPass.use();
            // 绑定 G-Buffer 纹理到 0/1/2
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpecular);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, lGainTexture);
            // 传入光源矩阵
            if (uLocMat4LightProj_lPass != -1)
                glUniformMatrix4fv(uLocMat4LightProj_lPass, 1,GL_FALSE, glm::value_ptr(mat4LightProj));
            if (uLocMat4LightView_lPass != -1)
                glUniformMatrix4fv(uLocMat4LightView_lPass, 1,GL_FALSE, glm::value_ptr(mat4LightView));

            // 设置观察位置与光源（示例：1 个点光源）
            GLint uLocViewPos = glGetUniformLocation(lPassProgId, "viewPos");
            if (uLocViewPos != -1) glUniform3fv(uLocViewPos, 1, glm::value_ptr(cam->cameraPos));

            GLint uLocNumLights = glGetUniformLocation(lPassProgId, "numLights");
            if (uLocNumLights != -1) glUniform1i(uLocNumLights, 1);
            // lights[0]
            GLint uLocL0Pos = glGetUniformLocation(lPassProgId, "lights[0].position");
            GLint uLocL0Color = glGetUniformLocation(lPassProgId, "lights[0].color");
            GLint uLocL0Const = glGetUniformLocation(lPassProgId, "lights[0].constant");
            GLint uLocL0Linear = glGetUniformLocation(lPassProgId, "lights[0].linear");
            GLint uLocL0Quad = glGetUniformLocation(lPassProgId, "lights[0].quadratic");
            if (uLocL0Pos != -1) glUniform3f(uLocL0Pos, 2.0f, 2.0f, 2.0f);
            if (uLocL0Color != -1) glUniform3f(uLocL0Color, 1.0f, 1.0f, 1.0f);
            if (uLocL0Const != -1) glUniform1f(uLocL0Const, 1.0f);
            if (uLocL0Linear != -1) glUniform1f(uLocL0Linear, 0.09f);
            if (uLocL0Quad != -1) glUniform1f(uLocL0Quad, 0.032f);

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);

            glfwSwapBuffers(hWindow);
        }
    }
    catch (const std::exception& e) {
        std::cerr<<"[Main] Exception: " << e.what() << std::endl;
        return -1;
    }
}