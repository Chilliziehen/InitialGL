//
// Created by 30367 on 2025/10/28.
//
#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "model\Model.h"

// 删除本地 STB 实现，改由 ModelTexture.cpp 提供实现
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
#include "config\config.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "FragmentShader.h"
#include "VertexShader.h"
#include "camera/Camera.h"
#include "program/Program.h"
#include "call_back/CallBack.h"
#include "light/LightManager.h"
#include "predefinition/Predefinitions.h"
#include "uniformMapper/UniformMapper.h"

#define WIDTH 1920
#define HEIGHT 1080

#define SHADOW_WIDTH 8192
#define SHADOW_HEIGHT 8192

#define WINDOW_TITLE "Render"

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

        GLFWwindow* hWindow = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE, nullptr, nullptr);
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

        // 移除手动纹理对象创建，改为使用 ModelTexture 抽象
        // GLuint texture, texture_specular; ... 删除

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

        UniformMapper uniformMapper;
        const auto toTextureUnitIndex = [](GLenum unit) -> GLint {
            return static_cast<GLint>(unit - GL_TEXTURE0);
        };

        // gPass 采样器绑定到纹理单元 0/1
        gPass.use();
        uniformMapper.setUniform(gPassProgId, "texture_diffuse1", toTextureUnitIndex(TEXTURE_DIFFUSE_INDEX));
        uniformMapper.setUniform(gPassProgId, "texture_specular1", toTextureUnitIndex(TEXTURE_SPECULAR_INDEX));

        // lPass 采样器固定绑定到纹理单元 0/1/2
        lPass.use();
        uniformMapper.setUniform(lPassProgId, "gPosition", 0);
        uniformMapper.setUniform(lPassProgId, "gNormal", 1);
        uniformMapper.setUniform(lPassProgId, "gAlbedoSpec", 2);
        uniformMapper.setUniform(lPassProgId, "shadowTexture", 3);
        uniformMapper.setUniform(lPassProgId, "gainTexture", 4);

        shadowPass.use();

        // 相机矩阵初值
        glm::mat4 mat4View = glm::lookAt(glm::vec3(1.0f,1.0f,3.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 mat4Proj = cam->getProjectionMatrix();
        glm::mat4 mat4Model = glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f));
        glm::mat4 mat4LightView = glm::lookAt(glm::vec3(2.0f,2.0f,2.0f),
            glm::vec3(0.0f,0.0f,0.0f),glm::normalize(glm::cross(glm::vec3(-1.0f,0.0f,1.0f),glm::vec3(-1.0f,-1.0f,-1.0f))));

        glm::mat4 mat4LightProj = glm::ortho(-15.0f,
        15.0f,
            -15.0f,
            8.0f,
            0.1f,
            100.0f);

        //glm::mat4 mat4LightProj = glm::perspective(glm::radians(60.0f),1.0f,0.1f,100.0f);
        gPass.use();
        uniformMapper.setUniform(gPassProgId, "umat4View", mat4View);
        uniformMapper.setUniform(gPassProgId, "umat4Proj", mat4Proj);
        uniformMapper.setUniform(gPassProgId, "umat4Model", mat4Model);

        glfwSetKeyCallback(hWindow, CallBack::keyboard_callback);
        glfwSetCursorPosCallback(hWindow, CallBack::mouse_callback);
        glfwSetScrollCallback(hWindow,CallBack::mouse_scroll_callback);

        Model teapot(std::string(RESOURCE_PATH)+std::string("/teamugobj.obj"));
        teapot.setModelMatrix(glm::translate(glm::mat4(1.0f),glm::vec3(0.3f,-0.2f,0.3f))*glm::scale(glm::mat4(1.0f),glm::vec3(0.5f,0.5f,0.5f)));
        teapot.load();

        Model plane(std::string(RESOURCE_PATH)+std::string("/plane.obj"));
        plane.load();
        plane.setModelMatrix(glm::translate(glm::mat4(1.0f),glm::vec3(-500.0f,-0.2f,-500.0f))*glm::scale(glm::mat4(1.0f),glm::vec3(1000.0f,1000.0f,1000.0f)));

        // 使用 ModelTexture 统一加载漫反射/高光贴图
        TextureCreateInfo texInfo{DEFAULT_TEXTURE_WRAP_S, DEFAULT_TEXTURE_WRAP_T, DEFAULT_TEXTURE_MIN_FILTER, DEFAULT_TEXTURE_MAG_FILTER};
        const std::string resourcePath = std::string(RESOURCE_PATH) + std::string("/fuckthisup.jpg");
        const auto loadModelTextures = [&](Model& model) {
            auto& diffuse = model.getDiffuseTextureObj();
            diffuse.setTextureIndex(TEXTURE_DIFFUSE_INDEX);
            diffuse.preloadTexture(resourcePath);
            diffuse.loadTexture(texInfo);
            diffuse.releaseMemory();

            auto& specular = model.getSpecularTextureObj();
            specular.setTextureIndex(TEXTURE_SPECULAR_INDEX);
            specular.preloadTexture(resourcePath);
            specular.loadTexture(texInfo);
            specular.releaseMemory();
        };
        loadModelTextures(teapot);
        loadModelTextures(plane);

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
        constexpr GLenum attachments[3] = {
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        //绑定到FBO的深度输出
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        //绑定到FBO的颜色输出
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lGainTexture, 0);
        constexpr GLenum sAttachments[1] = {GL_COLOR_ATTACHMENT0};
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
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
            glBindVertexArray(0);
        }

        // 初始化光源管理器并添加一个点光源
        LightManager lightManager;
        lightManager.add(Light{glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f});

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
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            shadowPass.use();
            //传入参数
            uniformMapper.setUniform(sPassProgId, "umat4LightView", mat4LightView);
            uniformMapper.setUniform(sPassProgId, "umat4LightProj", mat4LightProj);

            teapot.bindVAO();
            uniformMapper.setUniform(sPassProgId, "umat4Model", teapot.getModelMatrix());
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(teapot.getVertexCount()));
            teapot.unbindVAO();
            plane.bindVAO();
            uniformMapper.setUniform(sPassProgId, "umat4Model", plane.getModelMatrix());
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(plane.getVertexCount()));
            plane.unbindVAO();

            // 几何通道：写入 G-Buffer
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glViewport(0, 0, ViewWidth, ViewHeight);
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            gPass.use();
            uniformMapper.setUniform(gPassProgId, "umat4View", mat4View);
            uniformMapper.setUniform(gPassProgId, "umat4Proj", mat4Proj);

            // 使用模型自带的纹理对象进行绘制
            uniformMapper.setUniform(gPassProgId, "umat4Model", plane.getModelMatrix());
            plane.drawModel();

            uniformMapper.setUniform(gPassProgId, "umat4Model", teapot.getModelMatrix());
            teapot.drawModel();

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
            uniformMapper.setUniform(lPassProgId, "umat4LightProj", mat4LightProj);
            uniformMapper.setUniform(lPassProgId, "umat4LightView", mat4LightView);

            // 设置观察位置
            uniformMapper.setUniform(lPassProgId, "viewPos", cam->cameraPos);

            // 使用光源管理器上传光源数据（numLights 与 lights[i]）
            lightManager.upload(lPassProgId, "lights", "numLights");

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