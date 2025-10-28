//
// Created by 30367 on 2025/10/28.
//

#ifndef INC_2025_AUTUMN_CG_CAMERA_H
#define INC_2025_AUTUMN_CG_CAMERA_H
#include "glm.hpp"
//#include "GL/eglew.h"
#include "GLFW/glfw3.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
class Camera {
private:
    glm::mat4 viewMatrix;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;
public:
    GLfloat yaw;
    GLfloat pitch;
    Camera(glm::vec3, glm::vec3,glm::vec3);
    /*The first parameter tells the position of the camera,
    and the second one tells the direction of the camera.*/
    Camera();
    /*If user didn't give Camera parameters,
     *Initialize the object's matrix with default view matrix.*/
    ~Camera();
    /*Deconstruct the camera object*/
    glm::mat4 getViewMatrix();
    void refresh();
    void move(bool *keys);
    void mousemove();
};


#endif //INC_2025_AUTUMN_CG_CAMERA_H