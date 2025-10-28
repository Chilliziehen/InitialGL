//
// Created by 30367 on 2025/10/28.
//
#include "Camera.h"
#include "cmath"
const GLfloat MouseSensitivity = 0.25f;
const glm::mat4 defaultMat = glm::translate(defaultMat, glm::vec3(0.0f, 0.0f, -3.0f));
const glm::vec3 defaultPosition = glm::vec3(0.0f, 0.0f, 3.0f);
const glm::vec3 defaultLook = glm::vec3(0.0f, 0.0f, 0.0f);
const GLfloat defaultSpeed = 0.05f;
GLfloat xoffset, yoffset;
Camera::Camera()
{
    this->viewMatrix = defaultMat;
}
extern GLfloat xoffset;
extern GLfloat yoffset;
Camera::Camera(glm::vec3 position, glm::vec3 directionF,glm::vec3 directionU)//directionF tells the forward direction of the camera,and directionU tells the upward direction of the camera,
{

    this->cameraFront = glm::normalize(directionF);
    this->cameraPos = position;
    this->cameraUp = glm::normalize(directionU);
    this->cameraRight = glm::cross(this->cameraFront, this->cameraUp);
    this->viewMatrix = glm::lookAt(this->cameraPos,defaultLook, this->cameraRight);
    this->yaw = 0.0f;
    this->pitch = 0.0f;
}

Camera::~Camera(){}

void Camera::refresh()
{
    this->viewMatrix = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
}

void Camera::move(bool *keys)
{
    GLfloat speed = defaultSpeed;
    if(keys[GLFW_KEY_LEFT_SHIFT])
    {
        speed *= 0.4f;
    }
    if(keys[GLFW_KEY_W])
    {
        this->cameraPos += speed * this->cameraFront;
    }
    if(keys[GLFW_KEY_S])
    {
        this->cameraPos -= speed * this->cameraFront;
    }
    if(keys[GLFW_KEY_A])
    {
        this->cameraPos -= speed * this->cameraRight;
    }
    if(keys[GLFW_KEY_D])
    {
        this->cameraPos += speed * this->cameraRight;
    }
    this->refresh();
}


void Camera::mousemove()
{
    this->yaw += xoffset;
    this->pitch += yoffset;
    if(this->pitch>=89.0f)
    {
        this->pitch = 89.0f;
    }
    if (this->pitch <= -89.0f)
    {
        this->pitch = -89.0f;
    }
    this->cameraFront = glm::normalize(glm::vec3(std::sin(this->yaw),-std::sin(this->pitch), -std::cos(this->yaw)));
    xoffset = 0.0f;
    yoffset = 0.0f;
}

glm::mat4 Camera::getViewMatrix()
{
    return this->viewMatrix;
}